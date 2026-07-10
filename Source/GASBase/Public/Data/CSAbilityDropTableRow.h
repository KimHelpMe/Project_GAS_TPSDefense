#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CSAbilityDropTableRow.generated.h"

class UCSAbilityData;

USTRUCT(BlueprintType)
struct FCSAbilityDropTableRow : public FTableRowBase
{
    GENERATED_BODY()

    // 드랍할 스킬 DataAsset (기존 DA_Fireball 등을 여기에 연결)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UCSAbilityData> AbilityData;

    // 가중치 (높을수록 자주 드랍. 확률 = 이 값 / 전체 합)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Weight = 1.0f;

    // 이 웨이브부터 드랍 풀에 포함 (0이면 항상)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 MinWave = 0;
};