

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMCShotgunDamage.generated.h"


UCLASS()
class GASBASE_API UMMCShotgunDamage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
    // 에디터에서 BP(GE)마다 다른 Curve Table 지정 가능
    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    TSoftObjectPtr<UCurveTable> DamageFalloffCurve;

    // Curve Table의 Row 이름 (어떤 행을 읽을지)
    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    FName FalloffRowName = FName("ShotgunFalloff");
};
