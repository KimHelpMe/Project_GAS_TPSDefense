// CSK

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CSDropSettings.generated.h"

class UDataTable;

UCLASS(DefaultConfig, Config=Game)
class GASBASE_API UCSDropSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// 스킬 드랍 테이블 (DT_AbilityDropTable)
	UPROPERTY(EditAnywhere, Config, Category = "Drop")
	TSoftObjectPtr<UDataTable> AbilityDropTable;

	virtual FName GetCategoryName() const override { return TEXT("Game"); }
};
