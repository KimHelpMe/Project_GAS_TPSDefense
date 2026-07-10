


#include "Data/CSAbilityDropSubsystem.h"
#include "Data/CSAbilityDropTableRow.h"
#include "Abilities/CSAbilityData.h"
#include "Engine/DataTable.h"

void UCSAbilityDropSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 에셋 경로 지정 (에디터에서 Copy Reference로 확인)
	AbilityDropTable = LoadObject<UDataTable>(nullptr, TEXT("/Script/Engine.DataTable'/Game/Data/DT_AbilityDropTable.DT_AbilityDropTable'"));
}

UCSAbilityData* UCSAbilityDropSubsystem::RollRandomAbility(int32 CurrentWave) const
{
	if (!IsValid(AbilityDropTable)) return nullptr;

	// 전체 Row 가져오기
	TArray<FCSAbilityDropTableRow*> AllRows;
	AbilityDropTable->GetAllRows<FCSAbilityDropTableRow>(TEXT("RollRandom"), AllRows);

	// 현재 웨이브 기준으로 필터링 + 가중치 합산
	TArray<FCSAbilityDropTableRow*> ValidRows;
	float TotalWeight = 0.f;

	for (FCSAbilityDropTableRow* Row : AllRows)
	{
		if (!IsValid(Row->AbilityData)) continue;
		if (Row->MinWave > CurrentWave) continue;

		ValidRows.Add(Row);
		TotalWeight += Row->Weight;
	}

	if (ValidRows.IsEmpty() || TotalWeight <= 0.f) return nullptr;

	// 가중치 기반 랜덤 선택
	float Roll = FMath::FRandRange(0.f, TotalWeight);
	float Accumulate = 0.f;

	for (FCSAbilityDropTableRow* Row : ValidRows)
	{
		Accumulate += Row->Weight;
		if (Roll <= Accumulate)
		{
			return Row->AbilityData;
		}
	}

	// fallback
	return ValidRows.Last()->AbilityData;
}
