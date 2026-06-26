// CSK


#include "Characters/CSPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/CSPlayerState.h"
#include "AbilitySystem/CSAbilitySystemComponent.h"



ACSPlayerCharacter::ACSPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	// 컨트롤러 회전을 직접 적용하지 않음
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 이동 방향으로 캐릭터가 자동으로 회전
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

}

UAbilitySystemComponent* ACSPlayerCharacter::GetAbilitySystemComponent() const
{
	// CSPlayerState로 캐스트하여 ASC 반환
	ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(GetPlayerState());
	if (!IsValid(CSPlayerState)) return nullptr;

	return CSPlayerState->GetAbilitySystemComponent();
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
	// 때문에 InitAbilityActorInfo와 달리
	// 이 함수는 클라이언트와 서버 양쪽에서 호출할 필요가 없다.
	GiveStartupAbilities();
}

void ACSPlayerCharacter::OnRep_PlayerState()
{
	// 클라이언트 전용: PlayerState가 복제된 시점에 초기화

	Super::OnRep_PlayerState();

	if (!IsValid(GetAbilitySystemComponent())) return;
	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
}
