// CSK

#include "Game/CSTransitionGameMode.h"
#include "Player/CSBasePlayerController.h"

ACSTransitionGameMode::ACSTransitionGameMode()
{
	bUseSeamlessTravel = true;
}

void ACSTransitionGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	// PC가 LV_Transition에 완전히 등록된 시점 — 연출 시작 RPC 발송
	if (ACSBasePlayerController* PC = Cast<ACSBasePlayerController>(C))
	{
		PC->Client_StartTransitionCinematic();
	}

	// 첫 번째 플레이어 진입 시 한 번만 타이머 시작
	// (멀티플레이어에서 플레이어마다 HandleSeamlessTravelPlayer가 호출되므로 중복 방지)
	if (!bTravelTimerStarted)
	{
		bTravelTimerStarted = true;
		GetWorldTimerManager().SetTimer(
			TransitionTimerHandle,
			this,
			&ThisClass::TravelToGameMap,
			TransitionDuration,
			false
		);
	}
}

void ACSTransitionGameMode::TravelToGameMap()
{
	if (UWorld* World = GetWorld())
	{
		World->ServerTravel(FString::Printf(TEXT("%s?listen"), *GameMapPath));
	}
}
