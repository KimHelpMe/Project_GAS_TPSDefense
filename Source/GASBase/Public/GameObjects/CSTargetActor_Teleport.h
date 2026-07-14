#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor_SingleLineTrace.h"
#include "CSTargetActor_Teleport.generated.h"

/**
 * SingleLineTrace 기반 텔레포트 타겟팅 액터.
 * 기본 트레이스가 아무것도 못 맞췄을 때(하늘을 보는 경우), 그 지점에서 수직으로
 * 아래로 한 번 더 트레이스해서 바닥에 스냅되도록 보정한다.
 */
UCLASS(Blueprintable)
class GASBASE_API ACSTargetActor_Teleport : public AGameplayAbilityTargetActor_SingleLineTrace
{
	GENERATED_BODY()

public:
	virtual FHitResult PerformTrace(AActor* InSourceActor) override;

	// 하늘을 봐서 트레이스가 안 맞았을 때, 대체 수직 트레이스를 시작할 높이(캐릭터 위치 기준 위쪽 오프셋)
	UPROPERTY(EditDefaultsOnly, Category = "Teleport|GroundFallback")
	float FallbackTraceStartHeight = 500.f;

	// 대체 수직 트레이스의 최대 하강 거리
	UPROPERTY(EditDefaultsOnly, Category = "Teleport|GroundFallback")
	float FallbackTraceDownDistance = 100000.f;
};