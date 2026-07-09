


#include "GameMode/LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

ALobbyGameMode::ALobbyGameMode()
{
	// 심리스 트래블 활성화 필수 설정
	bUseSeamlessTravel = true;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// .Get()을 거쳐 오리지널 GameState 확보
	AGameStateBase* MyGameState = GameState.Get();
	if (MyGameState)
	{
		// GameState가 복제 중인 PlayerArray 배열 개수를 통해 현재 실시간 인원 수 파악
		int32 NumberOfPlayers = MyGameState->PlayerArray.Num();

		if (GEngine)
		{
			// 고정 Key 값 '1'을 사용하여 화면 스팸을 막고 숫자만 실시간 업데이트 갱신 처리
			GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Yellow, FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers));
		}

		// 지정된 인원(예: 4명)이 모이면 본 게임 매치 맵으로 자동 서버 트래블 발동
		if (NumberOfPlayers >= 4)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				// ?listen 옵션을 붙여 새 맵도 리슨 서버 상태로 오픈
				World->ServerTravel(TEXT("/Game/SHT/Maps/SHTMap?listen"));
			}
		}
	}

	// 새로 접속한 유저의 PlayerState를 추적하여 이름 획득 및 공지
	if (NewPlayer)
	{
		APlayerState* MyPlayerState = NewPlayer->GetPlayerState<APlayerState>();
		if (MyPlayerState)
		{
			FString PlayerName = MyPlayerState->GetPlayerName();
			if (GEngine)
			{
				// Key '-1'을 사용하여 유저가 들어올 때마다 누적 공지 출력
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("%s has joined the game!"), *PlayerName));
			}
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	AGameStateBase* MyGameState = GameState.Get();
	if (MyGameState)
	{
		// 로그아웃 시점에는 아직 PlayerArray에서 제거되기 직전일 수 있으므로 
		// 실시간 인원 예측 보정을 위해 임시로 1을 빼서 화면에 출력합니다 (디버깅 편의용)
		int32 NumberOfPlayers = MyGameState->PlayerArray.Num() - 1;

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Yellow, FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers));
		}
	}

	if (Exiting)
	{
		APlayerState* MyPlayerState = Exiting->GetPlayerState<APlayerState>();
		if (MyPlayerState)
		{
			FString PlayerName = MyPlayerState->GetPlayerName();
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("%s has exited the game."), *PlayerName));
			}
		}
	}
}
