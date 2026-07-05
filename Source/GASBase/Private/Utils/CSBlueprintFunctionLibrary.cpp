// CSK


#include "Utils/CSBlueprintFunctionLibrary.h"


EHitDirection UCSBlueprintFunctionLibrary::GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator)
{
	// 내적: 두 벡터가 얼마나 같은 방향을 향하는지 (-1 ~ 1)
	const float Dot = FVector::DotProduct(TargetForward, ToInstigator);

	// 각도 120°~ 180° -> 공격자가 후방에 있음
	if (Dot < -0.5f)
	{
		return EHitDirection::Back;
	}

	// 각도 60°~ 120° -> Back도 Forward도 아님, 좌/우 판별 필
	if (Dot < 0.5f)
	{
		// 외적의 Z 성분 부호로 좌/우를 구분
		const FVector Cross = FVector::CrossProduct(TargetForward, ToInstigator);
		return Cross.Z < 0.f ? EHitDirection::Left : EHitDirection::Right;
	}

	// 각도 0°~ 60° -> 공격자가 전방에 있음
	return EHitDirection::Front;
}

FName UCSBlueprintFunctionLibrary::GetHitDirectionName(const EHitDirection& HitDirection)
{
	switch (HitDirection)
	{
	case EHitDirection::Left: 
		return FName("Left");

	case EHitDirection::Right: 
		return FName("Right");

	case EHitDirection::Front: 
		return FName("Front");

	case EHitDirection::Back: 
		return FName("Back");

	default: return FName("None");
	}
}
