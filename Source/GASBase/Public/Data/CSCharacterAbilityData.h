

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "CSCharacterAbilityData.generated.h"

UCLASS()
class GASBASE_API UCSCharacterAbilityData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Montage")
	UAnimMontage* GetMontage(FGameplayTag AbilityTag) const;
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> MontageMap;
};
