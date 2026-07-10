
#include "Abilities/CSAbilityData.h"
#include "Engine/CurveTable.h"

float UCSAbilityData::GetValueAtLevel(FName StatName, int32 Level) const
{
    if (!IsValid(AbilityCurveTable)) return 0.f;

    const FRealCurve* Curve = AbilityCurveTable->FindCurve(StatName, TEXT(""));
    if (!Curve) return 0.f;

    return Curve->Eval(static_cast<float>(Level));
}
