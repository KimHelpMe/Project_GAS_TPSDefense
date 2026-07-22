// CSK

#include "AbilitySystem/CSGE_Heal.h"
#include "AbilitySystem/CSAttributeSet.h"
#include "GameplayTags/CSGameplayTags.h"

UCSGE_Heal::UCSGE_Heal()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	Period = FScalableFloat(TickInterval);

	FGameplayModifierInfo Modifier;
	Modifier.Attribute = UCSAttributeSet::GetHealthAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SetByCallerMagnitude;
	SetByCallerMagnitude.DataTag = CSGameplayTags::Data::Heal_Amount;
	Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerMagnitude);

	Modifiers.Add(Modifier);
}
