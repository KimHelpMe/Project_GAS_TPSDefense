// CSK

#include "Player/CSPlayerController.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/CSAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/CSGameplayTags.h"
#include "UI/ChatWidget.h"
#include "UI/CSRespawnWidget.h"

void ACSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());

		// 인게임 채팅 위젯 생성
		if (ChatWidgetClass)
		{
			InGameChatWidget = CreateWidget<UChatWidget>(this, ChatWidgetClass);
			if (InGameChatWidget)
			{
				InGameChatWidget->AddToViewport();
			}
		}

		// 리스폰 카운트다운 위젯 생성
		if (RespawnWidgetClass)
		{
			RespawnWidget = CreateWidget<UCSRespawnWidget>(this, RespawnWidgetClass);
			if (RespawnWidget)
			{
				RespawnWidget->AddToViewport();
			}
		}
	}
}

UChatWidget* ACSPlayerController::GetChatWidget() const
{
	return InGameChatWidget;
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
	EnhancedInputComponent->BindAction(TAction, ETriggerEvent::Started, this, &ThisClass::TestAbility);
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

	// 무기를 소환한 상태일 때만 FireSpawnedWeapons 이벤트를 보낸다.
	// 그렇지 않으면 GA_FireSpawnWeapon이 매 LMB마다 불필요하게 트리거되어
	// 빈 배열 bailout 분기에서 없는 태그를 지우려다 로그 에러가 발생한다.
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if (IsValid(ASC) && ASC->HasMatchingGameplayTag(CSGameplayTags::Status::WeaponsSpawned))
	{
		ServerSendGameplayEvent(CSGameplayTags::Abilities::FireSpawnedWeapons);
	}
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
	//FGameplayEventData Payload;
	//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), CSGameplayTags::Abilities::Q, Payload);
}

void ACSPlayerController::E()
{
	ActivateAbility(CSGameplayTags::Abilities::E);
	//FGameplayEventData Payload;
	//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), CSGameplayTags::Abilities::E, Payload);
}

void ACSPlayerController::R()
{
	ActivateAbility(CSGameplayTags::Abilities::R);
	//FGameplayEventData Payload;
	//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), CSGameplayTags::Abilities::R, Payload);
}

void ACSPlayerController::TestAbility()
{
	ActivateAbility(CSGameplayTags::Abilities::TestAbility);
	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), CSGameplayTags::Abilities::TestAbility, Payload);
	ServerSendGameplayEvent(CSGameplayTags::Abilities::TestAbility);
}
