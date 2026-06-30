// CSK
// aa

#include "Abilities/CSGameplayAbility.h"
#include "CSDebugHelper.h"

void UCSGameplayAbility::DebugDrawString(const FString& Msg)
{
	if (bDrawDebugs)
	{
		Debug::Print(Msg);
	}
}

void UCSGameplayAbility::DebugDrawSphere(const FVector& Center, float Radius, FLinearColor Color, float Duration)
{
	if (bDrawDebugs)
	{
		Debug::DrawSphere(this, Center, Radius, Color.ToFColor(true), Duration);
	}
}
