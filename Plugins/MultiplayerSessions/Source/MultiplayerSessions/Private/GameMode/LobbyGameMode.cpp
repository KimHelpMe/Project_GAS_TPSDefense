// CSK

#include "GameMode/LobbyGameMode.h"
#include "MultiplayerSessionsSubsystem.h"
#include "LobbyTransitionInterface.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "EngineUtils.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	// StartSession 완료 콜백 바인딩
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UMultiplayerSessionsSubsystem* Subsystem = GI->GetSubsystem<UMultiplayerSessionsSubsystem>())
		{
			Subsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSessionComplete);
		}
	}
}

void ALobbyGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Subsystem은 GameInstance에 붙어서 맵 전환 후에도 살아있으므로
	// GameMode 파괴 전에 델리게이트 바인딩을 명시적으로 해제
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UMultiplayerSessionsSubsystem* Subsystem = GI->GetSubsystem<UMultiplayerSessionsSubsystem>())
		{
			Subsystem->MultiplayerOnStartSessionComplete.RemoveDynamic(this, &ThisClass::OnStartSessionComplete);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

void ALobbyGameMode::OnStartSessionComplete(bool bWasSuccessful)
{
	// 모든 클라이언트에 달리기 연출 시작 지시
	NotifyAllPlayersStartTransition();

	// Wipe + 달리기 실루얣 연출 시간(3초) 후 ServerTravel
	FTimerHandle TravelTimerHandle;
	GetWorldTimerManager().SetTimer(
		TravelTimerHandle,
		[this]()
		{
			if (UWorld* World = GetWorld())
			{
				World->ServerTravel(FString::Printf(TEXT("%s?listen"), *GameMapPath));
			}
		},
		3.0f,
		false
	);
}

void ALobbyGameMode::NotifyAllPlayersStartTransition()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			if (PC->Implements<ULobbyTransitionInterface>())
			{
				ILobbyTransitionInterface::Execute_StartBattleTransition(PC);
			}
		}
	}
}
