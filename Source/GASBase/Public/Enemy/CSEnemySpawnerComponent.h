// CSK

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/CSEnemySpawnTableRow.h"
#include "CSEnemySpawnerComponent.generated.h"

class ACSEnemyCharacter;
class UDataTable;

// 적이 하나 스폰될 때마다 브로드캐스트 (RegisterEnemy, pathActor 설정 등 후속 처리는 이 델리게이트를 구독해서 처리)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemySpawned, ACSEnemyCharacter*, NewEnemy, AActor*, SpawnLine);

// 진행 중인 라인 하나의 스폰 상태
USTRUCT()
struct FCSActiveLineSpawnState
{
	GENERATED_BODY()

	UPROPERTY()
	FCSEnemyLineSpawnInfo LineInfo;

	int32 SpawnedCount = 0;

	FTimerHandle TimerHandle;
};

// 데이터 테이블(FCSEnemySpawnTableRow) 기반으로 라운드/라인별 적 스폰만 담당하는 컴포넌트
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GASBASE_API UCSEnemySpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCSEnemySpawnerComponent();

	// 해당 라운드에 속한 모든 라인의 스폰을 동시에 시작
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void StartRound(int32 Round);

	// 진행 중인 모든 라인의 스폰을 중단
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void StopSpawning();

	UPROPERTY(BlueprintAssignable, Category = "Spawn")
	FOnEnemySpawned OnEnemySpawned;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TObjectPtr<UDataTable> SpawnTable;

protected:
	void SpawnFromLine(int32 LineIndex);

private:
	UPROPERTY()
	TArray<FCSActiveLineSpawnState> ActiveLines;
};
