// CSK

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CSRespawnWidget.generated.h"

/**
 * 리스폰 카운트다운 위젯
 * 사망 시 화면에 남은 시간을 표시하고, 리스폰 시 자동으로 숨긴다.
 * 비주얼은 BP에서 구현한다.
 */
UCLASS()
class GASBASE_API UCSRespawnWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// BP에서 카운트다운 텍스트 업데이트 (매 틱 호출)
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Countdown Update"))
	void BP_OnCountdownUpdate(float RemainingSeconds);

	// BP에서 위젯 표시/숨김 처리 (상태 전환 시에만 호출)
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Respawn State Changed"))
	void BP_OnRespawnStateChanged(bool bIsDead);

private:
	// 이전 프레임의 사망 상태 — 상태 전환 감지용
	bool bWasDead = false;
};
