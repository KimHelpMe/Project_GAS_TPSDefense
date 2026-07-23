// CSK

#pragma once

#include "CoreMinimal.h"
#include "CSMatchResult.generated.h"

/**
 * 매치 종료 시 각 플레이어에게 전달되는 결과 구조체
 * 승리/패배 공통으로 사용하며, 나중에 필드 추가만으로 확장 가능
 */
USTRUCT(BlueprintType)
struct FCSMatchResult
{
	GENERATED_BODY()

	// 이번 매치에서 처치한 적 수
	UPROPERTY(BlueprintReadOnly)
	int32 KillCount = 0;

	// 이번 매치에서 가한 총 데미지
	UPROPERTY(BlueprintReadOnly)
	float TotalDamageDealt = 0.f;
};
