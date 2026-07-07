// CSK
// aa

#include "Abilities/CSGameplayAbility.h"
#include "CSDebugHelper.h"

void UCSGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	DebugDrawString(FString::Printf(TEXT("%s Activated"), *GetName()), 3.f);
}


void UCSGameplayAbility::DebugDrawString(const FString& Msg, float Duration, FLinearColor Color, int32 Inkey)
{
	if (bDrawDebugs)
	{
		Debug::Print(Msg, Duration, Color.ToFColor(true), Inkey);
	}
}

void UCSGameplayAbility::DebugDrawSphere(const FVector& Center, float Radius, FLinearColor Color, float Duration)
{
	if (bDrawDebugs)
	{
		Debug::DrawSphere(this, Center, Radius, Color.ToFColor(true), Duration);
	}
}

// 어빌리티가 자기 슬롯 태그를 물어봄.
FGameplayTag UCSGameplayAbility::GetAssignedSlotTag() const
{
	if (const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
	{
		for (const FGameplayTag& Tag : Spec->GetDynamicSpecSourceTags())
		{
			return Tag;
		}
	}
	return FGameplayTag::EmptyTag;
}