#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CSEnemySpawnTableRow.generated.h"

class ACSEnemyCharacter;

// 라인 하나에 대한 스폰 정보
USTRUCT(BlueprintType)
struct FCSEnemyLineSpawnInfo
{
    GENERATED_BODY()

    // 레벨에 배치된 라인 액터(Bp_EnemyPath_line 등) 참조
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<AActor> SpawnLine;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<ACSEnemyCharacter> EnemyClass;

    // 이 라인에서 스폰할 적 수
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Count = 1;

    // 스폰 간격(초)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float SpawnInterval = 1.0f;
};

// 데이터 테이블 로우 = 라운드 1개, 라운드 안에 여러 라인을 배열로 관리
USTRUCT(BlueprintType)
struct FCSEnemySpawnTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Round = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FCSEnemyLineSpawnInfo> Lines;
};
