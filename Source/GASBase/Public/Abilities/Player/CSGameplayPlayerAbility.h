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
	FVector GetAimpoint();

	// GetAimpoint()와 동일한 트레이스를 수행하되, ImpactNormal/bBlockingHit 등 전체 결과를 반환한다.
	// (기존 GetAimpoint() 호출부는 그대로 두고, Normal이 필요한 곳에서만 이 함수를 쓰면 됨)
	UFUNCTION(BlueprintCallable, Category = "GAS|Aiming")
	FHitResult GetAimpointHitResult();

	UPROPERTY(BlueprintReadOnly, Category = "GAS|Aiming")
	float TraceDistance = 100000.f;



private:
	//�����ϰ� Player�� �ּҸ� ����(ĳ��)�ص� �𸮾� ����Ʈ ������ ����
	TWeakObjectPtr<ACSPlayerCharacter> CachedPlayerCharacter;

	//�����ϰ� Player ��Ʈ�ѷ��� �ּҸ� ����(ĳ��)�ص� �𸮾� ����Ʈ ������ ���� 
	TWeakObjectPtr<ACSPlayerController> CachedPlayerController;
};
