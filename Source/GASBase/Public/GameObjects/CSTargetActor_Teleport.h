#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor_SingleLineTrace.h"
#include "CSTargetActor_Teleport.generated.h"

UCLASS()
class GASBASE_API ACSTargetActor_Teleport : public AGameplayAbilityTargetActor_SingleLineTrace 
{
	GENERATED_BODY()

public:
	virtual FHitResult PerformTrace(AActor* InSourceActor) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite , Category = "Teleport")
	float FallbackTraceStartHeight = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Teleport")
	float FallbackTraceDownDistance = 2000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Teleport")
	float TeleportRange = 1500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Teleport")
	float WallPushOffset = 15.f;  // ���� �¾��� �� �� ǥ�鿡�� �о�� ���� �Ÿ�
};