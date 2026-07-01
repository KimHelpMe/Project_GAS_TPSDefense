// CSK


#include "Abilities/CSGameplayAbility.h"
#include "CSDebugHelper.h"

void UCSGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// bDrawDebugs가 true이고 GEngine이 유효할 때만 활성화 메시지 출력
	if (bDrawDebugs && IsValid(GEngine))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("%s Activated"), *GetName()));
	}

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
