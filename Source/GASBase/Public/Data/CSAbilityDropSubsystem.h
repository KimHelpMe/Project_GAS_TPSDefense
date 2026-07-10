

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CSAbilityDropSubsystem.generated.h"

class UCSAbilityData;
class UDataTable;

UCLASS()
class GASBASE_API UCSAbilityDropSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 현재 웨이브 기준으로 가중치 랜덤 뽑기
	UFUNCTION(BlueprintCallable, Category = "Drop")
	UCSAbilityData* RollRandomAbility(int32 CurrentWave = 0) const;

private:
	UPROPERTY()
	TObjectPtr<UDataTable> AbilityDropTable;
};
