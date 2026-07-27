// Stone

#pragma once

#include "CoreMinimal.h"
#include "Abilities/CSGameplayAbility.h"
#include "CSGameplayPlayerAbility.generated.h"

class ACSPlayerCharacter;
class ACSPlayerController;
/**
 * 
 */
UCLASS()
class GASBASE_API UCSGameplayPlayerAbility : public UCSGameplayAbility
{
	GENERATED_BODY()
	
public:
	//Player ĳ���͸� ������ ����
	UFUNCTION(BlueprintPure, Category = "GAS|Abilities")
	ACSPlayerCharacter* GetPlayerCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "GAS|Abilities")
	ACSPlayerController* GetPlayerControllerFromActorInfo();

	UFUNCTION(BlueprintCallable, Category = "GAS|Aiming")
	FVector GetAimpoint(bool bIgnorePawns = false);

	// GetAimpoint()와 동일한 트레이스를 수행하되, ImpactNormal/bBlockingHit 등 전체 결과를 반환한다.
	// (기존 GetAimpoint() 호출부는 그대로 두고, Normal이 필요한 곳에서만 이 함수를 쓰면 됨)
	UFUNCTION(BlueprintCallable, Category = "GAS|Aiming")
	FHitResult GetAimpointHitResult(bool bIgnorePawns = false);

	// GetAimpointHitResult()의 다중 발사 버전.
	// PelletCount 수만큼 SpreadAngleDegrees 범위 안에서 랜덤 방향으로 각각 트레이스를 수행한다.
	// 맞지 않은 탄도 ImpactPoint가 최대 사거리 끝점으로 채워져 반환된다. (VFX 끝점 활용 가능)
	UFUNCTION(BlueprintCallable, Category = "GAS|Aiming")
	TArray<FHitResult> GetSpreadHitResults(int32 PelletCount, float SpreadAngleDegrees, FName MuzzleSocketName, float MaxRange);

	UPROPERTY(BlueprintReadOnly, Category = "GAS|Aiming")
	float TraceDistance = 100000.f;



private:
	//�����ϰ� Player�� �ּҸ� ����(ĳ��)�ص� �𸮾� ����Ʈ ������ ����
	TWeakObjectPtr<ACSPlayerCharacter> CachedPlayerCharacter;

	//�����ϰ� Player ��Ʈ�ѷ��� �ּҸ� ����(ĳ��)�ص� �𸮾� ����Ʈ ������ ���� 
	TWeakObjectPtr<ACSPlayerController> CachedPlayerController;
};
