// CSK


#include "Player/CSPlayerController.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/CSGameplayTags.h"
#include "Characters/CSBaseCharacter.h"


void ACSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Enhanced Input 서브시스템 가져오기
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!IsValid(InputSubsystem)) return;

	// 배열의 모든 IMC를 우선순위 0으로 등록
	for (UInputMappingContext* IMC : InputMappingContexts)
	{
		InputSubsystem->AddMappingContext(IMC, 0);
	}

	// EnhancedInputComponent로 캐스팅
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EnhancedInputComponent)) return;

	// 입력 액션 → 콜백 바인딩
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::StopJumping);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Started, this, &ThisClass::Roll);

	EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Triggered, this, &ThisClass::Primary);
	EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Started, this, &ThisClass::Secondary);

	EnhancedInputComponent->BindAction(QAction, ETriggerEvent::Started, this, &ThisClass::Q);
}

void ACSPlayerController::ActivateAbility(const FGameplayTag& AbilityTag) const
{
	if (!IsAlive()) return;

	// Blueprint의 "Get Ability System Component" 노드와 동일한 함수
	// 내부적으로 전달받은 액터가 IAbilitySystemInterface를 구현하는지 확인하고, 
	// 구현한다면 GetAbilitySystemComponent()를 호출하여 반환
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if (!IsValid(ASC)) return;

	ASC->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer());
}


void ACSPlayerController::Jump()
{
	if (!IsAlive()) return;

	if (!IsValid(GetCharacter())) return;
	GetCharacter()->Jump();
}

void ACSPlayerController::StopJumping()
{
	if (!IsAlive()) return;

	if (!IsValid(GetCharacter())) return;
	GetCharacter()->StopJumping();
}

void ACSPlayerController::Move(const FInputActionValue& Value)
{
	if (!IsAlive()) return;

	if (!IsValid(GetPawn())) return;

	// 입력 값을 2D 벡터로 가져옴 
	const FVector2D MovementVector = Value.Get<FVector2D>();

	// 컨트롤러 Yaw만 (pitch, yaw, roll)
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

	// Yaw 회전으로부터 전방/우측 벡터 계산
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	GetPawn()->AddMovementInput(ForwardDirection, MovementVector.Y);
	GetPawn()->AddMovementInput(RightDirection, MovementVector.X);
}

void ACSPlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();

	AddYawInput(LookVector.X);
	AddPitchInput(LookVector.Y);
}

void ACSPlayerController::Roll()
{
	ActivateAbility(CSGameplayTags::Abilities::Roll);
}

void ACSPlayerController::Primary()
{
	ActivateAbility(CSGameplayTags::Abilities::Primary);
}

void ACSPlayerController::Secondary()
{
	ActivateAbility(CSGameplayTags::Abilities::Secondary);

}

void ACSPlayerController::Q()
{
	ActivateAbility(CSGameplayTags::Abilities::Q);

}

bool ACSPlayerController::IsAlive() const
{
	// 빙의 중인 폰을 CC_BaseCharacter로 캐스팅
	ACSBaseCharacter* BaseCharacter = Cast<ACSBaseCharacter>(GetPawn());

	// 폰이 유효하지 않으면 사망으로 간주
	if (!IsValid(BaseCharacter)) return false;

	// 캐릭터의 생존 여부 반환
	return BaseCharacter->IsAlive();
}