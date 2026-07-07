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
	//Player 캐릭터를 빠르게 리턴
	UFUNCTION(BlueprintPure, Category = "Ability")
	ACSPlayerCharacter* GetCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Ability")
	ACSPlayerController* GetPlayerControllerFromActorInfo();

	UFUNCTION(BlueprintCallable, Category = "GAS|Aiming")
	FVector GetAimpoint();

	UPROPERTY(BlueprintReadOnly, Category = "GAS|Aiming")
	float TraceDistance = 100000.f;



private:
	//안전하게 Player의 주소를 보관(캐싱)해둘 언리얼 스마트 포인터 서랍
	TWeakObjectPtr<ACSPlayerCharacter> CachedCharacter;

	//안전하게 Player 컨트롤러의 주소를 보관(캐싱)해둘 언리얼 스마트 포인터 서랍 
	TWeakObjectPtr<ACSPlayerController> CachedController;
};
