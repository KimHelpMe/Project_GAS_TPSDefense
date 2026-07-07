#include "Abilities/CSAbilityPickup.h"
#include "Abilities/CSAbilityData.h"
#include "AbilitySystem/CSAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

ACSAbilityPickup::ACSAbilityPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = Mesh;

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetSphereRadius(75.f);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ACSAbilityPickup::BeginPlay()
{
	Super::BeginPlay();

	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ACSAbilityPickup::OnOverlapBegin);
}

void ACSAbilityPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	// 서버 권위 필수: TryPickupAbility가 GiveAbility/AddToAbilityLevel을 호출하므로
	// 클라이언트에서 실행되면 무시되고 리플리케이션 꼬임만 생김
	if (!HasAuthority() || PossibleAbilities.IsEmpty()) return;

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	UCSAbilitySystemComponent* CSASC = Cast<UCSAbilitySystemComponent>(ASC);
	if (!IsValid(CSASC)) return;

	const int32 RandomIndex = FMath::RandRange(0, PossibleAbilities.Num() - 1);
	const bool bPickedUp = CSASC->TryPickupAbility(PossibleAbilities[RandomIndex]);

	if (bPickedUp)
	{
		Destroy(); // 실패 시(슬롯 꽉 참 + 미보유 스킬)엔 Destroy 안 함 → "못 주워"
	}
}