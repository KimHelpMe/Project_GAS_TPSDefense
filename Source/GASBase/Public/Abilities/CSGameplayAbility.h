// CSK

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CSGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GASBASE_API UCSGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugDrawString(const FString& Msg);
	
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugDrawSphere(const FVector& Center, float Radius = 50.f, FLinearColor Color = FLinearColor::Red, float Duration = 3.f);
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
	bool bDrawDebugs = false;


};
