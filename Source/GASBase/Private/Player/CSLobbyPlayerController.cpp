// CSK

#include "Player/CSLobbyPlayerController.h"
#include "Lobby/LobbyInteractionComponent.h"
#include "UI/LobbyWidget.h"
#include "UI/ChatWidget.h"
#include "MultiplayerSessionsSubsystem.h"
#include "GameFramework/GameStateBase.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"

ACSLobbyPlayerController::ACSLobbyPlayerController()
{
	LobbyInteractionComponent = CreateDefaultSubobject<ULobbyInteractionComponent>(TEXT("LobbyInteractionComponent"));
}

void ACSLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 기본 상태: 커서 비활성, 게임 입력 (이동 가능)
	if (IsLocalController())
	{
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
	}

	// 로비 위젯 생성
	if (IsLocalController() && LobbyWidgetClass)
	{
		LobbyWidget = CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);
		if (LobbyWidget)
		{
			LobbyWidget->AddToViewport();
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
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EnhancedInputComponent)) return;

	if (ToggleCursorAction)
	{
		EnhancedInputComponent->BindAction(ToggleCursorAction, ETriggerEvent::Started, this, &ThisClass::ToggleCursor);
	}
}

void ACSLobbyPlayerController::ToggleCursor()
{
	// 채팅 활성 중이면 먼저 채팅 비활성
	if (bChatActive)
	{
		DeactivateChat();
		return;
	}

	// 커서 토글
	bShowMouseCursor = !bShowMouseCursor;

	if (bShowMouseCursor)
	{
		FInputModeGameAndUI InputMode;
		SetInputMode(InputMode);
	}
	else
	{
		SetInputMode(FInputModeGameOnly());
	}
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
