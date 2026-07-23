// CSK

#include "UI/CSRespawnWidget.h"
#include "Player/CSPlayerState.h"

void UCSRespawnWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	APlayerController* PC = GetOwningPlayer();
	if (!IsValid(PC)) return;

	ACSPlayerState* PS = PC->GetPlayerState<ACSPlayerState>();
	if (!IsValid(PS)) return;

	const bool bIsDead = PS->RespawnTime > 0.f;

	// 상태 전환 감지 (살아있음 ↔ 사망)
	if (bIsDead != bWasDead)
	{
		bWasDead = bIsDead;
		BP_OnRespawnStateChanged(bIsDead);
	}

	// 사망 중일 때만 카운트다운 업데이트
	if (bIsDead)
	{
		// 복제받은 시점부터 로컬 경과 시간으로 남은 시간 계산
		const float Elapsed = GetWorld()->GetTimeSeconds() - PS->GetLocalRespawnStartTime();
		const float Remaining = FMath::Max(0.f, PS->GetLocalRespawnDuration() - Elapsed);
		BP_OnCountdownUpdate(Remaining);
	}
}
