// CSK

#pragma once

#include "CoreMinimal.h"
#include "Abilities/CSGameplayAbility.h"
#include "CSHitReact.generated.h"

/**
 * 적 캐릭터용 Hit React 어빌리티
 * 이벤트를 수신하여 피격 반응을 처리한다
 */

UCLASS()
class GASBASE_API UCSHitReact : public UCSGameplayAbility
{
	GENERATED_BODY()
	
public:
	// 아바타의 forward 벡터와 공격자를 향하는 벡터를 캐싱하는 함수
	UFUNCTION(BlueprintCallable, Category = "GAS|Abilities")
	void CacheHitDirectionVectors(AActor* Instigator);

protected:
	// 피격당하는 아바타(적)의 forward 벡터
	UPROPERTY(BlueprintReadOnly, Category = "GAS|Abilities")
	FVector AvatarForward;

	// 아바타에서 공격자를 향하는 정규화된 벡터
	UPROPERTY(BlueprintReadOnly, Category = "GAS|Abilities")
	FVector ToInstigator;
};
