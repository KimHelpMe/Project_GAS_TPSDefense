// CSK


#include "Player/CSPlayerController.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/CSAbilitySystemComponent.h"
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

	EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Triggered, this, &ThisClass::LMB);
	EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Started, this, &ThisClass::RMB);


	EnhancedInputComponent->BindAction(QAction, ETriggerEvent::Started, this, &ThisClass::Q);
	EnhancedInputComponent->BindAction(EAction, ETriggerEvent::Started, this, &ThisClass::E);
	EnhancedInputComponent->BindAction(RAction, ETriggerEvent::Started, this, &ThisClass::R);
	EnhancedInputComponent->BindAction(TAction, ETriggerEvent::Started, this, &ThisClass::T);
}


void ACSPlayerController::ActivateAbility(const FGameplayTag& AbilityTag) const
{	//if (!IsAlive()) return;

	// Blueprint의 "Get Ability System Component" 노드와 동일한 함수
	// 내부적으로 전달받은 액터가 IAbilitySystemInterface를 구현하는지 확인하고, 
	// 구현한다면 GetAbilitySystemComponent()를 호출하여 반환
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	UCSAbilitySystemComponent* CSASC = Cast<UCSAbilitySystemComponent>(ASC);
	if (!IsValid(CSASC)) return;

	// Q/E/R/T 등 "슬롯 스킬"은 슬롯 핸들로 활성화
	if (!CSASC->TryActivateAbilityBySlot(AbilityTag))
	{
		// Primary/Secondary/Roll처럼 슬롯과 무관한 고정 어빌리티는 기존 방식 유지 (AssetTag 매칭)
		CSASC->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer());
	}
}



void ACSPlayerController::Jump()
{
	if (!IsValid(GetCharacter())) return;
	if (!IsAlive()) return;
	GetCharacter()->Jump();
}

void ACSPlayerController::StopJumping()
{
	if (!IsValid(GetCharacter())) return;
	if (!IsAlive()) return;
	GetCharacter()->StopJumping();
}

void ACSPlayerController::Move(const FInputActionValue& Value)
{
	if (!IsValid(GetPawn())) return;
	if (!IsAlive()) return;

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

void ACSPlayerController::LMB()
{
	ActivateAbility(CSGameplayTags::Abilities::Primary);
}

void ACSPlayerController::RMB()
{
	ActivateAbility(CSGameplayTags::Abilities::Secondary);

}

void ACSPlayerController::Q()
{
	ActivateAbility(CSGameplayTags::Abilities::Q);
	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), CSGameplayTags::Abilities::Q, Payload);
}

void ACSPlayerController::E()
{
	ActivateAbility(CSGameplayTags::Abilities::E);
	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), CSGameplayTags::Abilities::E, Payload);
}

void ACSPlayerController::R()
{
	ActivateAbility(CSGameplayTags::Abilities::Fireball);
	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), CSGameplayTags::Abilities::R, Payload);
}

void ACSPlayerController::T()
{
	ActivateAbility(CSGameplayTags::Abilities::T);
	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), CSGameplayTags::Abilities::T, Payload);
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