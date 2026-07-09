// CSK

#include "Player/CSPlayerController.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/CSAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/CSGameplayTags.h"

void ACSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 인게임: 커서 없음, 게임 전용 입력 모드
	if (IsLocalController())
	{
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
	}
}

void ACSPlayerController::SetupInputComponent()
{
	// 베이스 클래스에서 IMC 등록 + 이동 입력 바인딩 처리
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EnhancedInputComponent)) return;

	// 어빌리티 전용 입력 바인딩 추가
	EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Started, this, &ThisClass::Roll);
	EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Triggered, this, &ThisClass::LMB);
	EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Started, this, &ThisClass::RMB);
	EnhancedInputComponent->BindAction(QAction, ETriggerEvent::Started, this, &ThisClass::Q);
	EnhancedInputComponent->BindAction(EAction, ETriggerEvent::Started, this, &ThisClass::E);
	EnhancedInputComponent->BindAction(RAction, ETriggerEvent::Started, this, &ThisClass::R);
	EnhancedInputComponent->BindAction(TAction, ETriggerEvent::Started, this, &ThisClass::T);
}

void ACSPlayerController::ActivateAbility(const FGameplayTag& AbilityTag) const
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	UCSAbilitySystemComponent* CSASC = Cast<UCSAbilitySystemComponent>(ASC);
	if (!IsValid(CSASC)) return;

	if (!CSASC->TryActivateAbilityBySlot(AbilityTag))
	{
		CSASC->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer());
	}
}

void ACSPlayerController::Roll()
{
	ActivateAbility(CSGameplayTags::Abilities::Roll);
}

void ACSPlayerController::LMB()
{
	ActivateAbility(CSGameplayTags::Abilities::Primary);

	FGameplayEventData Payload;
	Payload.Instigator = GetPawn();
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), CSGameplayTags::Abilities::Long_Primary, Payload);
	ServerSendGameplayEvent(CSGameplayTags::Abilities::Long_Primary);
}

void ACSPlayerController::ServerSendGameplayEvent_Implementation(FGameplayTag EventTag)
{
	FGameplayEventData Payload;
	Payload.Instigator = GetPawn();
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), EventTag, Payload);
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
