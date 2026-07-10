#include "Abilities/CSAbilityPickup.h"
#include "Abilities/CSAbilityData.h"
#include "AbilitySystem/CSAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/CSSkillInfoWidget.h"
#include "Net/UnrealNetwork.h"
#include "Data/CSAbilityDropSubsystem.h"


ACSAbilityPickup::ACSAbilityPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	// 리플리케이트 필수: 이게 꺼져있으면 서버에서 Destroy()해도 클라이언트엔
	// 그 사실이 전달되는 채널 자체가 없어서, 서버 화면에서만 사라지고
	// 클라이언트 화면엔 박스가 그대로 남아있게 됨. AssignedAbility 리플리케이션에도 필요.
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	// QueryOnly + Visibility 채널만 Block: 플레이어를 물리적으로 막지는 않지만(NoCollision과 달리),
	// 크로스헤어 라인트레이스(ECC_Visibility)는 이 메시에 맞아야 조준 감지가 가능해짐
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	RootComponent = Mesh;

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetSphereRadius(75.f);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// 박스 위에 뜨는 스킬 정보 팝업. 카메라를 항상 향하도록 Screen 스페이스로 설정.
	InfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InfoWidgetComponent"));
	InfoWidgetComponent->SetupAttachment(RootComponent);
	InfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	InfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	// 컴팩트 가로형 스킬 정보 카드(WBP_SkillInfoPopup) 크기에 맞춤
	InfoWidgetComponent->SetDrawSize(FVector2D(280.f, 130.f));
	InfoWidgetComponent->SetVisibility(false); // 기본 숨김 - 크로스헤어로 조준할 때만 보임
}

void ACSAbilityPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 서버가 스폰 시점에 정한 스킬을 모든 클라이언트에 복제 -> 각자 화면에서 크로스헤어 UI에 표시 가능해짐
	DOREPLIFETIME(ACSAbilityPickup, AssignedAbility);
}

void ACSAbilityPickup::BeginPlay()
{
	Super::BeginPlay();

	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ACSAbilityPickup::OnOverlapBegin);

	// 스폰 시점에 딱 한 번 랜덤 결정 (서버 권위). 습득 시엔 이 값을 재추첨하지 않고 그대로 사용.
	if (HasAuthority())
	{
		if (bUseDropTable)
		{
			// 드랍 테이블에서 가중치 랜덤
			if (UCSAbilityDropSubsystem* DropSub = GetGameInstance()->GetSubsystem<UCSAbilityDropSubsystem>())
			{
				AssignedAbility = DropSub->RollRandomAbility(SpawnWave);
			}
		}
		else if (!PossibleAbilities.IsEmpty())
		{
			// 기존 방식 유지
			const int32 RandomIndex = FMath::RandRange(0, PossibleAbilities.Num() - 1);
			AssignedAbility = PossibleAbilities[RandomIndex];
		}
	}

	// 서버(리슨 서버의 로컬 클라이언트 포함)는 OnRep이 호출되지 않으므로 직접 갱신
	UpdateInfoWidgetContent();
}

void ACSAbilityPickup::OnRep_AssignedAbility()
{
	// 클라이언트는 리플리케이트 도착 시 이쪽에서 위젯 내용을 채움
	UpdateInfoWidgetContent();
}

void ACSAbilityPickup::UpdateInfoWidgetContent()
{
	if (!IsValid(AssignedAbility) || !IsValid(InfoWidgetComponent)) return;

	if (UCSSkillInfoWidget* Widget = Cast<UCSSkillInfoWidget>(InfoWidgetComponent->GetUserWidgetObject()))
	{
		Widget->Setup(AssignedAbility);
	}
}

void ACSAbilityPickup::SetInfoWidgetVisible(bool bVisible)
{
	if (IsValid(InfoWidgetComponent))
	{
		InfoWidgetComponent->SetVisibility(bVisible);
	}
}

void ACSAbilityPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	// 서버 권위 필수: TryPickupAbility가 GiveAbility/AddToAbilityLevel을 호출하므로
	// 클라이언트에서 실행되면 무시되고 리플리케이션 꼬임만 생김
	if (!HasAuthority() || !IsValid(AssignedAbility)) return;

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	UCSAbilitySystemComponent* CSASC = Cast<UCSAbilitySystemComponent>(ASC);
	if (!IsValid(CSASC)) return;

	// 스폰 시점에 이미 정해진 스킬을 그대로 사용 (재추첨하지 않음)
	const bool bPickedUp = CSASC->TryPickupAbility(AssignedAbility);

	if (bPickedUp)
	{
		Destroy(); // 실패 시(슬롯 꽉 참 + 미보유 스킬)엔 Destroy 안 함 → "못 주워"
	}
}