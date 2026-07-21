

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "InGameMode.generated.h"


UCLASS()
class GASBASE_API AInGameMode : public AGameMode
{
	GENERATED_BODY()

public:
    AInGameMode();

    // 캐릭터 사망 시 호출 — 리스폰 타이머 시작
    void OnCharacterDied(AController* DeadController);

    // "플레이어 스폰(Spawn/생성) 프로세스"를 제어하는 함수
    virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

protected:
    // SeamlessTravel 완료 후 호출 — 클라이언트에 EndBattleTransition 전달
    virtual void PostSeamlessTravel() override;

    // 타이머 완료 시 호출 — 실제 리스폰 수행
    void RespawnCharacter(AController* Controller);

protected:
    // 에디터에서 조정 가능한 리스폰 딜레이
    UPROPERTY(EditDefaultsOnly, Category = "Respawn")
    float RespawnDelay = 20.f;

    // 컨트롤러별 리스폰 타이머 관리
    TMap<TWeakObjectPtr<AController>, FTimerHandle> RespawnTimers;
};
