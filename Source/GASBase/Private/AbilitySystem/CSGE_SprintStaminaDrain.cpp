// 스프린트 시 초당 스태미나 소모

#include "AbilitySystem/CSGE_SprintStaminaDrain.h"
#include "AbilitySystem/CSAttributeSet.h"

namespace
{
	constexpr float SprintStaminaDrainPeriod = 0.2f;
	constexpr float SprintStaminaDrainPerSecond = 30d.f;
}

UCSGE_SprintStaminaDrain::UCSGE_SprintStaminaDrain()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	Period = FScalableFloat(SprintStaminaDrainPeriod);

	FGameplayModifierInfo Modifier;
	Modifier.Attribute = UCSAttributeSet::GetStaminaAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;
	Modifier.ModifierMagnitude = FScalableFloat(-SprintStaminaDrainPerSecond * SprintStaminaDrainPeriod);
	Modifiers.Add(Modifier);
}
