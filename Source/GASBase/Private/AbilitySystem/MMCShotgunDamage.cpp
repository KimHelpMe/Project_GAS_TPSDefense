


#include "AbilitySystem/MMCShotgunDamage.h"
#include "GameplayTags/CSGameplayTags.h"



float UMMCShotgunDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float BaseDamage = Spec.GetSetByCallerMagnitude(CSGameplayTags::Data::Damage_Base, false, 0.f);

	const float Distance = Spec.GetSetByCallerMagnitude(CSGameplayTags::Data::Damage_Distance, false, 0.f);

	UCurveTable* CurveTable = DamageFalloffCurve.LoadSynchronous();
	if (!CurveTable || FalloffRowName.IsNone())
	{
		return -BaseDamage;
	}

	const FRealCurve* Curve = CurveTable->FindCurve(FalloffRowName, TEXT(""));
	if (!Curve)
	{
		return -BaseDamage;
	}

	const float Multiplier = Curve->Eval(Distance);
	return -BaseDamage * FMath::Clamp(Multiplier, 0.f, 1.f);
}
