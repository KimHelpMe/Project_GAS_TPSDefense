

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Game/CSMatchResult.h"
#include "InGameMode.generated.h"


UCLASS()
class GASBASE_API AInGameMode : public AGameMode
{
	GENERATED_BODY()

public:
    AInGameMode();

    // 캐릭터 사망 시 호출 — 리스폰 타이머 시작
    void OnCharacterDied(AController* DeadController);

    // 보스 사망 시 호출 — 승리 판정 및 모든 플레이어에게 결과 전송
    void OnBossDied();

    // "플레이어 스폰(Spawn/생성) 프로세스"를 제어하는 함수
    virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

protected:
    // SeamlessTravel 중 PC 교체 완료 직후 호출 — EndBattleTransition 전달
    virtual void HandleSeamlessTravelPlayer(AController*& C) override;

    // 타이머 완료 시 호출 — 실제 리스폰 수행
    void RespawnCharacter(AController* Controller);

protected:
    // 에디터에서 조정 가능한 리스폰 딜레이
    UPROPERTY(EditDefaultsOnly, Category = "Respawn")
    float RespawnDelay = 20.f;

    // 승리 후 로비 복귀까지 대기 시간
    UPROPERTY(EditDefaultsOnly, Category = "Victory")
    float VictoryLobbyDelay = 10.f;

    // 컨트롤러별 리스폰 타이머 관리
    TMap<TWeakObjectPtr<AController>, FTimerHandle> RespawnTimers;

private:
    // 모든 플레이어를 로비로 이동
    void ReturnAllPlayersToLobby();

    // OnBossDied 중복 호출 방지 플래그
    bool bVictoryTriggered = false;
};
