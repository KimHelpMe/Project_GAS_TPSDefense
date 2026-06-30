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
	* 그러나 두 점이 같다면 실질적으로 이동이 없으므로 Trace가 아닌 Overlap Test를 사용하는 것이 올바르다
	*/

	/** C++ 전용 최적화 패턴
	* 블루프린트의 오버랩 노드는 기본적으로 모든 콜리전 채널에 대해 응답을 평가
	* 따라서 결과를 필터링하려면 캐스팅이나 인터페이스 체크 같은 추가 작업이 필요
	* 반면 C++에서는 FCollisionResponseParams를 직접 구성하여 쿼리 자체가 특정 오브젝트 타입만 감지하도록 제한 가능
	*/

	UFUNCTION(BlueprintCallable, Category = "GAS|Abilities")
	void HitBoxOverlap();

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
