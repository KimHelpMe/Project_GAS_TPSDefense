// CSK

#include "Player/CSLobbyPlayerController.h"
#include "Lobby/LobbyInteractionComponent.h"
#include "UI/LobbyWidget.h"
#include "UI/ChatWidget.h"
#include "MultiplayerSessionsSubsystem.h"
#include "GameFramework/GameStateBase.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Camera/CameraActor.h"
#include "EngineUtils.h"

ACSLobbyPlayerController::ACSLobbyPlayerController()
{
	LobbyInteractionComponent = CreateDefaultSubobject<ULobbyInteractionComponent>(TEXT("LobbyInteractionComponent"));
}

void ACSLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 캐릭터 선택창: 커서 활성, UI 전용 입력 모드
	if (IsLocalController())
	{
		// 태그 "LobbyCamera"가 붙은 CameraActor를 뷰 타겟으로 설정
		for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
		{
			if (It->ActorHasTag(TEXT("LobbyCamera")))
			{
				SetViewTargetWithBlend(*It);
				break;
			}
		}

		bShowMouseCursor = true;
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
	}

	// 로비 위젯 생성
	if (IsLocalController() && LobbyWidgetClass)
	{
		LobbyWidget = CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);
		if (LobbyWidget)
		{
			LobbyWidget->AddToViewport();

			// 위젯 생성 후 포커스 지정 — 클릭 없이도 Enter 입력 바로 받음
			FInputModeGameAndUI InputMode;
			InputMode.SetHideCursorDuringCapture(false);
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetWidgetToFocus(LobbyWidget->TakeWidget());
			SetInputMode(InputMode);
		}
	}

	// 인원 폴링 타이머
	if (IsLocalController())
	{
		GetWorldTimerManager().SetTimer(
			PlayerCountTimerHandle,
			this,
			&ThisClass::PollPlayerCount,
			1.0f,
			true
		);
	}

	// 호스트: 시작 요청 델리게이트 바인딩
	if (HasAuthority() && LobbyInteractionComponent)
	{
		LobbyInteractionComponent->OnGameStartRequested.AddDynamic(this, &ThisClass::OnGameStartRequested);
	}
}

void ACSLobbyPlayerController::SetupInputComponent()
{
	// 베이스의 이동 입력 바인딩 스킵, 채팅 Enter만 바인딩
	APlayerController::SetupInputComponent();

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!IsValid(InputSubsystem)) return;

	for (UInputMappingContext* IMC : InputMappingContexts)
	{
		InputSubsystem->AddMappingContext(IMC, 0);
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EIC)) return;

	// 채팅 Enter만 바인딩
	if (ChatActivateAction)
	{
		EIC->BindAction(ChatActivateAction, ETriggerEvent::Started, this, &ThisClass::ActivateChat);
	}
}

void ACSLobbyPlayerController::OnChatDeactivated()
{
	if (!LobbyWidget) return;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(LobbyWidget->TakeWidget());
	SetInputMode(InputMode);
}

UChatWidget* ACSLobbyPlayerController::GetChatWidget() const
{
	if (LobbyWidget)
	{
		return LobbyWidget->GetChatWidget();
	}
	return nullptr;
}

void ACSLobbyPlayerController::UpdateLobbyPlayerCount(int32 Count)
{
	if (LobbyWidget)
	{
		LobbyWidget->UpdatePlayerCount(Count);
	}
}

void ACSLobbyPlayerController::PollPlayerCount()
{
	UWorld* World = GetWorld();
	if (!World) return;

	AGameStateBase* GS = World->GetGameState<AGameStateBase>();
	if (!GS) return;

	int32 CurrentCount = GS->PlayerArray.Num();

	if (CurrentCount != CachedPlayerCount)
	{
		CachedPlayerCount = CurrentCount;
		UpdateLobbyPlayerCount(CurrentCount);
	}
}

void ACSLobbyPlayerController::RequestLeaveLobby()
{
	// 폴링 타이머 정리
	GetWorldTimerManager().ClearTimer(PlayerCountTimerHandle);

	// 세션 파괴
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UMultiplayerSessionsSubsystem* Subsystem = GI->GetSubsystem<UMultiplayerSessionsSubsystem>())
		{
			Subsystem->DestroySession();
		}
	}

	// 타이틀 맵으로 이동
	ClientTravel(TEXT("/MultiplayerSessions/Maps/Main"), ETravelType::TRAVEL_Absolute);
}

void ACSLobbyPlayerController::StartBattleTransition_Implementation()
{
	// 서버에서 Execute_StartBattleTransition 호출 → 실제 연출은 클라이언트에서
	Client_StartBattleTransition();
}

void ACSLobbyPlayerController::Client_StartBattleTransition_Implementation()
{
	// 달리기 연출 시작 — BP에서 구현
	BP_StartBattleTransition();
}

void ACSLobbyPlayerController::OnGameStartRequested()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UMultiplayerSessionsSubsystem* Subsystem = GI->GetSubsystem<UMultiplayerSessionsSubsystem>())
		{
			Subsystem->StartSession();
		}
	}
}
