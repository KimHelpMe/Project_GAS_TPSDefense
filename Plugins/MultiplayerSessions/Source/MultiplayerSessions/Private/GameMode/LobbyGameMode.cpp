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

	// 현재 인원 수 표시
	if (AGameStateBase* GS = GameState.Get())
	{
		int32 NumberOfPlayers = GS->PlayerArray.Num();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Yellow,
				FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers));
		}
	}

	// 접속 공지
	if (NewPlayer)
	{
		if (APlayerState* PS = NewPlayer->GetPlayerState<APlayerState>())
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan,
					FString::Printf(TEXT("%s has joined the game!"), *PS->GetPlayerName()));
			}
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	// Logout 시점에는 아직 PlayerArray에서 제거 전이므로 -1 보정
	if (AGameStateBase* GS = GameState.Get())
	{
		int32 NumberOfPlayers = GS->PlayerArray.Num() - 1;
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Yellow,
				FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers));
		}
	}

	if (Exiting)
	{
		if (APlayerState* PS = Exiting->GetPlayerState<APlayerState>())
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red,
					FString::Printf(TEXT("%s has exited the game."), *PS->GetPlayerName()));
			}
		}
	}
}

void ALobbyGameMode::OnStartSessionComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, TEXT("StartSession failed - proceeding with travel anyway."));
	}

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
