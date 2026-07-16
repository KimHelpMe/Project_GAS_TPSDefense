// CSK


#include "Characters/CSPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/CSPlayerState.h"
#include "AbilitySystem/CSAbilitySystemComponent.h"
#include "AbilitySystem/CSAttributeSet.h"
#include "Abilities/CSAbilityPickup.h"
#include "GameFramework/PlayerController.h"
#include "Game/InGameMode.h"
#include "Camera/CSCameraFadeComponent.h"



ACSPlayerCharacter::ACSPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	// 컨트롤러 회전을 직접 적용하지 않음
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// 이동 방향으로 캐릭터가 자동으로 회전
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 800.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 230.f;
	CameraBoom->SocketOffset = FVector(0.f, 100.f, 100.f);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	CameraFadeComponent = CreateDefaultSubobject<UCSCameraFadeComponent>(TEXT("CameraFadeComponent"));
}

UAbilitySystemComponent* ACSPlayerCharacter::GetAbilitySystemComponent() const
{
	// CSPlayerState로 캐스트하여 ASC 반환
	ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(GetPlayerState());
	if (!IsValid(CSPlayerState)) return nullptr;

	return CSPlayerState->GetAbilitySystemComponent();
}

UAttributeSet* ACSPlayerCharacter::GetAttributeSet() const
{
	ACSPlayerState* CCPlayerState = Cast<ACSPlayerState>(GetPlayerState());
	if (!IsValid(CCPlayerState)) return nullptr;

	return CCPlayerState->GetAttributeSet();
}

void ACSPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// ASC가 유효하지 않거나 서버가 아니면 조기 반환
	if (!IsValid(GetAbilitySystemComponent()) || !HasAuthority()) return;

	// 서버 전용: Owner = PlayerState, Avatar = 이 캐릭터
	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);

	// 어빌리티 부여는 서버에서만 수행
	// 서버에서 GiveAbility를 호출하면 클라이언트에는 자동으로 복제(replicate)
	GiveStartupAbilities();
	InitializeAttributes();
	ApplyStartupEffects();

	// 어트리뷰트 변경 델리게이트를 바인딩
	BindAttributeDelegates();
}

void ACSPlayerCharacter::OnRep_PlayerState()
{
	// 클라이언트 전용: PlayerState가 복제된 시점에 초기화

	Super::OnRep_PlayerState();

	if (!IsValid(GetAbilitySystemComponent())) return;
	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);

	// 어트리뷰트 변경 델리게이트를 바인딩
	BindAttributeDelegates();

}

void ACSPlayerCharacter::BindAttributeDelegates()
{
	//  초기화 완료 시점을 알리는 델리게이트
	OnASCInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());

	const UCSAttributeSet* AttributeSet = Cast<UCSAttributeSet>(GetAttributeSet());
	if (!IsValid(AttributeSet)) return;

	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
}

void ACSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 로컬 컨트롤 여부는 UpdateCrosshairTrace 내부에서 매번 체크한다.
	// BeginPlay 시점엔 클라이언트 쪽 Controller 빙의가 아직 안 끝났을 수 있어서,
	// 여기서 IsLocallyControlled()로 미리 걸러버리면 타이머 자체가 안 걸릴 위험이 있음.
	GetWorldTimerManager().SetTimer(CrosshairTraceTimerHandle, this, &ACSPlayerCharacter::UpdateCrosshairTrace, 0.1f, true);
}

void ACSPlayerCharacter::HandleDeath_Implementation()
{
	Super::HandleDeath_Implementation();

	bUseControllerRotationYaw = false;

	// 서버에서만 GameMode에 리스폰 요청
	if (HasAuthority())
	{
		if (AInGameMode* GM = GetWorld()->GetAuthGameMode<AInGameMode>())
		{
			GM->OnCharacterDied(GetController());
		}
	}
}

void ACSPlayerCharacter::HandleRespawn()
{
	Super::HandleRespawn();

	DisableRagdoll();
	bUseControllerRotationYaw = true;
}

void ACSPlayerCharacter::OnRep_Alive()
{
	Super::OnRep_Alive();

	if (IsAlive())
	{
		// 리스폰 — 클라이언트 측 복원
		DisableRagdoll();
		bUseControllerRotationYaw = true;
	}
	else
	{
		bUseControllerRotationYaw = false;
		EnableRagdoll();
	}
}

void ACSPlayerCharacter::UpdateCrosshairTrace()
{
	// 남의 화면에 내 조준 UI가 뜰 필요는 없으므로, 내가 직접 조작하는 캐릭터에서만 처리
	if (!IsLocallyControlled()) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!IsValid(PC)) return;

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector TraceEnd = CamLoc + CamRot.Vector() * 500.f; // 조준 사거리 5m, 필요시 조정

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, TraceEnd, ECC_Visibility, Params);

	ACSAbilityPickup* NewHovered = Cast<ACSAbilityPickup>(Hit.GetActor());

	// 조준 대상이 바뀌었을 때만 위젯 표시/숨김을 갱신 (매 틱마다 불필요하게 호출 안 하도록)
	if (HoveredPickup.Get() != NewHovered)
	{
		if (HoveredPickup.IsValid())
		{
			HoveredPickup->SetInfoWidgetVisible(false);
		}

		HoveredPickup = NewHovered;

		if (IsValid(NewHovered))
		{
			NewHovered->SetInfoWidgetVisible(true);
		}
	}
}
