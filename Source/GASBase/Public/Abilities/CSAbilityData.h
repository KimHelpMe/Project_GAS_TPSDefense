//THE

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CSAbilityData.generated.h"

class UGameplayAbility;
class UTexture2D;
class UCurveTable;;

// Skill Data Asset.

UCLASS(BlueprintType)
class GASBASE_API UCSAbilityData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 같은 스킬인지 판단 기준은 AbilityClass 비교로 충분함.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<UGameplayAbility> AbilityClass;

	// 신규 습득 시 시작 레벨.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	int32 AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|UI")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|UI")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|UI")
	FText Description;

#pragma region CSK/ CurveTable

	// 이 스킬 전용 CurveTable (CT_Fireball - Row 이름: Damage, Cooldown, Cost 등
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Data")
	TObjectPtr<UCurveTable> AbilityCurveTable;

	// 레벨별 수치 조회 (UI 표시용)
	// StatName: "Damage", "Cooldown", "Cost" 등
	UFUNCTION(BlueprintCallable, Category = "Ability|Data")
	float GetValueAtLevel(FName StatName, int32 Level) const;

#pragma endregion

};
