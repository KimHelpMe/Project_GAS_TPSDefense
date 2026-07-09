// CSK

#include "Player/CSLobbyPlayerController.h"
#include "Lobby/LobbyInteractionComponent.h"
#include "UI/LobbyWidget.h"
#include "MultiplayerSessionsSubsystem.h"
#include "GameFramework/GameStateBase.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"

ACSLobbyPlayerController::ACSLobbyPlayerController()
{
	// ChatComponent는 베이스 클래스(ACSBasePlayerController)에서 생성됨
	LobbyInteractionComponent = CreateDefaultSubobject<ULobbyInteractionComponent>(TEXT("LobbyInteractionComponent"));
}

void ACSLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 기본 상태: 커서 비활성, 게임 입력 모드 (이동 가능)
	if (IsLocalController())
	{
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
	}

	// 로비 위젯 생성 (로컬 클라이언트에서만)
	if (IsLocalController() && LobbyWidgetClass)
	{
		LobbyWidget = CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);
		if (LobbyWidget)
		{
			LobbyWidget->AddToViewport();
		}
	}

	// 인원 변화 감지용 타이머 (매 1초마다 GameState의 PlayerArray 확인)
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

	// 서버(호스트)에서만: LobbyInteractionComponent의 시작 요청 델리게이트를 Subsystem에 연결
	if (HasAuthority() && LobbyInteractionComponent)
	{
		LobbyInteractionComponent->OnGameStartRequested.AddDynamic(this, &ThisClass::OnGameStartRequested);
	}
}

void ACSLobbyPlayerController::SetupInputComponent()
{
	// 베이스에서 IMC 등록 + 이동 바인딩
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EnhancedInputComponent)) return;

	// UI 토글 바인딩
	if (ToggleCursorAction)
	{
		EnhancedInputComponent->BindAction(ToggleCursorAction, ETriggerEvent::Started, this, &ThisClass::ToggleCursor);
	}
}

void ACSLobbyPlayerController::ToggleCursor()
{
	bCursorActive = !bCursorActive;

	if (bCursorActive)
	{
		// 커서 활성: UI + 게임 동시 입력 + 채팅 입력창 포커스
		bShowMouseCursor = true;
		FInputModeGameAndUI InputMode;
		SetInputMode(InputMode);

		if (LobbyWidget)
		{
			LobbyWidget->FocusChatInput();
		}
	}
	else
	{
		// 커서 비활성: 게임 전용
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
	}
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
