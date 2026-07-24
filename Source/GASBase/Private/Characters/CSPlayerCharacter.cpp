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
#include "AbilitySystem/CSGE_SprintStaminaDrain.h"
#include "GameplayTags/CSGameplayTags.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"


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
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
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
	GetAbilitySystemComponent()->AddMinimalReplicationGameplayTag(CSGameplayTags::Team::Player);

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

	GetAbilitySystemComponent()->AddMinimalReplicationGameplayTag(CSGameplayTags::Team::Player);

}

void ACSPlayerCharacter::BindAttributeDelegates()
{
	//  초기화 완료 시점을 알리는 델리게이트
	OnASCInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());

	const UCSAttributeSet* AttributeSet = Cast<UCSAttributeSet>(GetAttributeSet());
	if (!IsValid(AttributeSet)) return;

	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);

	GetAbilitySystemComponent()->RegisterGameplayTagEvent(CSGameplayTags::Status::Sprinting, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnSprintingTagChanged);
}

void ACSPlayerCharacter::OnSprintingTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	// GE 적용/제거는 서버에서만 수행 (복제는 GAS가 자동 처리)
	if (!HasAuthority()) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;

	if (NewCount > 0)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(UCSGE_SprintStaminaDrain::StaticClass(), 1.f, ContextHandle);
		SprintStaminaDrainHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		// GE_StaminaRegen의 OngoingTagRequirements(IgnoreTags)가 이 태그를 보고 회복을 일시 정지시킴
		ASC->AddLooseGameplayTag(CSGameplayTags::Status::UsingStamina);
	}
	else
	{
		ASC->RemoveActiveGameplayEffect(SprintStaminaDrainHandle);
		ASC->RemoveLooseGameplayTag(CSGameplayTags::Status::UsingStamina);
	}
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
	}
}

void ACSPlayerCharacter::OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData)
{
	Super::OnHealthChanged(AttributeChangeData);

	// 남의 화면에 내 체력 이펙트가 뜨면 안 되니, 로컬 컨트롤 캐릭터일 때만 처리
	if (!IsLocallyControlled()) return;

	const UCSAttributeSet* AttributeSet = Cast<UCSAttributeSet>(GetAttributeSet());
	if (!IsValid(AttributeSet)) return;

	const float MaxHealth = AttributeSet->GetMaxHealth();
	if (MaxHealth <= 0.f) return;

	// 데미지 비율(1 - HP비율)을 거듭제곱해서 낮은 HP일수록 급격히 커지는 커브 적용
	const float HealthRatio = FMath::Clamp(AttributeChangeData.NewValue / MaxHealth, 0.f, 1.f);
	const float DamageRatio = 1.f - HealthRatio;
	const float VignetteIntensity = FMath::Pow(DamageRatio, DamageVignetteExponent);

	if (!IsValid(DamageVignetteMPC)) return;

	if (UMaterialParameterCollectionInstance* MPCInstance = GetWorld()->GetParameterCollectionInstance(DamageVignetteMPC))
	{
		MPCInstance->SetScalarParameterValue(DamageVignetteParamName, VignetteIntensity);

		const bool bIsDead = AttributeChangeData.NewValue <= 0.f;
		MPCInstance->SetScalarParameterValue(TEXT("IsDead"), bIsDead ? 1.f : 0.f);
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
