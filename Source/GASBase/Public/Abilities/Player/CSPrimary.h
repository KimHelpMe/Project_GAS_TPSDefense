// CSK

#pragma once

#include "CoreMinimal.h"
#include "Abilities/CSGameplayAbility.h"
#include "CSPrimary.generated.h"

/**
 * 
 */

UCLASS()
class GASBASE_API UCSPrimary : public UCSGameplayAbility
{
	GENERATED_BODY()

public:
	/** Sphere Trace vs Overlap 
	* 개발 현장에서 시작점과 끝점이 동일한 Sphere Trace를 사용하는 경우를 흔히 볼 수 있다
	* 그러나 두 점이 같다면 실질적으로 이동이 없으므로 Trace가 아닌 Overlap을 사용하는 것이 올바르다
	*/

	// 만약 나중에 더 많은 정보(예: 히트 위치, 노멀 벡터 등)를 블루프린트로 넘기고 싶다면, 
	// BlueprintType을 가진 커스텀 구조체를 만들어 필요한 데이터를 담아 반환
	UFUNCTION(BlueprintCallable, Category = "GAS|Abilities")
	TArray<AActor*>  HitBoxOverlap();


protected:
	void DrawHitBoxOverlapDebugs(const TArray<FOverlapResult>& OverlapResults, const FVector& HitBoxLocation) const;

private:
	// 구체의 반지름
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Abilities")
	float Radius = 100.f;

	// 전방으로 히트박스를 밀어낼 거리
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Abilities")
	float ForwardOffset = 200.f;

	// 높이 오프셋 (지면에서 살짝 띄움)
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Abilities")
	float ElevationOffset = 20.f;

};
