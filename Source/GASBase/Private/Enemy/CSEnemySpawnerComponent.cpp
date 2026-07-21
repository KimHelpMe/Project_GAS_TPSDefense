// CSK

#include "Enemy/CSEnemySpawnerComponent.h"
#include "Characters/CSEnemyCharacter.h"
#include "TimerManager.h"
#include "Engine/World.h"

UCSEnemySpawnerComponent::UCSEnemySpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCSEnemySpawnerComponent::StartRound(int32 Round)
{
	StopSpawning();

	if (!SpawnTable) return;

	UWorld* World = GetWorld();
	if (!World) return;

	FString ContextString;
	TArray<FCSEnemySpawnTableRow*> Rows;
	SpawnTable->GetAllRows<FCSEnemySpawnTableRow>(ContextString, Rows);

	const FCSEnemySpawnTableRow* RoundRow = nullptr;
	for (const FCSEnemySpawnTableRow* Row : Rows)
	{
		if (Row && Row->Round == Round)
		{
			RoundRow = Row;
			break;
		}
	}

	if (!RoundRow) return;

	for (const FCSEnemyLineSpawnInfo& LineInfo : RoundRow->Lines)
	{
		if (LineInfo.Count <= 0) continue;

		FCSActiveLineSpawnState NewState;
		NewState.LineInfo = LineInfo;
		ActiveLines.Add(NewState);

		const int32 LineIndex = ActiveLines.Num() - 1;

		World->GetTimerManager().SetTimer(
			ActiveLines[LineIndex].TimerHandle,
			FTimerDelegate::CreateUObject(this, &UCSEnemySpawnerComponent::SpawnFromLine, LineIndex),
			FMath::Max(LineInfo.SpawnInterval, 0.01f),
			true,
			0.0f);
	}
}

void UCSEnemySpawnerComponent::StopSpawning()
{
	if (UWorld* World = GetWorld())
	{
		for (FCSActiveLineSpawnState& State : ActiveLines)
		{
			World->GetTimerManager().ClearTimer(State.TimerHandle);
		}
	}

	ActiveLines.Empty();
}

void UCSEnemySpawnerComponent::SpawnFromLine(int32 LineIndex)
{
	if (!ActiveLines.IsValidIndex(LineIndex)) return;

	FCSActiveLineSpawnState& State = ActiveLines[LineIndex];

	UWorld* World = GetWorld();

	if (State.SpawnedCount >= State.LineInfo.Count || !State.LineInfo.EnemyClass || !World)
	{
		if (World)
		{
			World->GetTimerManager().ClearTimer(State.TimerHandle);
		}
		return;
	}

	AActor* LineActor = State.LineInfo.SpawnLine.LoadSynchronous();
	if (!LineActor) return;

	const FTransform SpawnTransform(LineActor->GetActorRotation(), LineActor->GetActorLocation());

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ACSEnemyCharacter* NewEnemy = World->SpawnActor<ACSEnemyCharacter>(State.LineInfo.EnemyClass, SpawnTransform, SpawnParams);
	State.SpawnedCount++;

	if (NewEnemy)
	{
		OnEnemySpawned.Broadcast(NewEnemy, LineActor);
	}
}
