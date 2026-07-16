// CSK

#include "Player/CSPlayerController.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/CSAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/CSGameplayTags.h"
#include "UI/ChatWidget.h"
#include "Kismet/GameplayStatics.h"

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

void ACSPlayerController::ClientShowGameOver_Implementation()
{
	if (!GameOverWidgetClass) return;

	UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
	if (!IsValid(GameOverWidget)) return;

	GameOverWidget->AddToViewport(10);

	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());

	// 패배 후에는 더 이상 조작이 필요 없으므로 이 컨트롤러의 모든 입력을 차단
	DisableInput(this);
}

void ACSPlayerController::ServerRequestRestart_Implementation()
{
	// 서버가 현재 레벨을 다시 열면 서버 트래블이 발생해 모든 클라이언트가 함께 로드됨
	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this);
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName), true);
}

void ACSPlayerController::ServerRequestReturnToLobby_Implementation()
{
	UGameplayStatics::OpenLevel(this, FName("/MultiplayerSessions/Maps/Lobby"), true);
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
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ThisClass::StartSprint);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ThisClass::StopSprint);
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
	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), CSGameplayTags::Abilities::Q, Payload);
	//FGameplayEventData Payload;
	//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), CSGameplayTags::Abilities::Q, Payload);
}

void ACSPlayerController::E()
{
	ActivateAbility(CSGameplayTags::Abilities::E);
	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), CSGameplayTags::Abilities::E, Payload);
	//FGameplayEventData Payload;
	//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), CSGameplayTags::Abilities::E, Payload);
}

void ACSPlayerController::R()
{
	ActivateAbility(CSGameplayTags::Abilities::R);
	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), CSGameplayTags::Abilities::R, Payload);
	//FGameplayEventData Payload;
	//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), CSGameplayTags::Abilities::R, Payload);
}

void ACSPlayerController::TestAbility()
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());

	// GA_Teleport가 이미 targeting~confirm 상태로 실행 중인지를,
	// 태그(HasMatchingGameplayTag) 대신 어빌리티 스펙 자체의 IsActive()로 판단한다.
	// Activation Owned Tag는 클라이언트 자기 자신에게 즉시/확실하게 반영된다는 보장이 약해서
	// (이 프로젝트의 AddLooseGameplayTag 관련 주석 참고: 루즈 태그는 원격 클라이언트에 복제 안 됨),
	// 리슨 서버의 클라이언트에서는 우연히 맞아도 원격 클라이언트에서는 확인 입력이 씹히는 문제가 있었다.
	// IsActive()는 각 클라/서버가 자기 로컬 예측 인스턴스 상태를 직접 들고 있는 값이라 안전하다.
	bool bAlreadyTeleporting = false;
	if (IsValid(ASC))
	{
		for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
		{
			if (IsValid(Spec.Ability) && Spec.Ability->GetAssetTags().HasTagExact(CSGameplayTags::Abilities::TestAbility))
			{
				bAlreadyTeleporting = Spec.IsActive();
				break;
			}
		}
	}

	if (!bAlreadyTeleporting)
	{
		ActivateAbility(CSGameplayTags::Abilities::TestAbility);
		return;
	}

	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), CSGameplayTags::Abilities::TestAbility, Payload);
	ServerSendGameplayEvent(CSGameplayTags::Abilities::TestAbility);
}

void ACSPlayerController::StartSprint()
{
	ActivateAbility(CSGameplayTags::Abilities::Sprint);
	
}

void ACSPlayerController::StopSprint()
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if (!IsValid(ASC)) return;

	FGameplayTagContainer SprintTagContainer = CSGameplayTags::Abilities::Sprint.GetTag().GetSingleTagContainer();
	ASC->CancelAbilities(&SprintTagContainer);
}
