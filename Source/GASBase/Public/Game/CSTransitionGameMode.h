// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CSTransitionGameMode.generated.h"

/**
 * LV_Transition 전용 GameMode
 * 모든 플레이어 연출 시작 후 N초 대기 → LV_CSK로 ServerTravel
 */
UCLASS()
class GASBASE_API ACSTransitionGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACSTransitionGameMode();

protected:
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

private:
	// 연출 대기 후 LV_CSK로 이동
	void TravelToGameMap();

	// 에디터에서 조정 가능한 연출 대기 시간
	UPROPERTY(EditDefaultsOnly, Category = "Transition")
	float TransitionDuration = 3.0f;

	// 목적지 게임 맵 경로
	UPROPERTY(EditDefaultsOnly, Category = "Transition")
	FString GameMapPath = TEXT("/Game/TaehosWork/Maps/LV_NewMap");

	FTimerHandle TransitionTimerHandle;

	// 모든 플레이어가 진입 완료됐을 때 한 번만 타이머 시작
	bool bTravelTimerStarted = false;
};
