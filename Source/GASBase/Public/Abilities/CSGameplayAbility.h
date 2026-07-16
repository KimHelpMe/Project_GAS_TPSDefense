// CSK

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CSGameplayAbility.generated.h"

class ACSBaseCharacter;

UCLASS()
class GASBASE_API UCSGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	// 어빌리티 활성화 시 공통 디버그 메시지를 출력하기 위한 오버라이드
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugDrawString(const FString& Msg, float Duration = 3.f, FLinearColor Color = FLinearColor::Green, int32 Inkey = -1);
	
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugDrawSphere(const FVector& Center, float Radius = 50.f, FLinearColor Color = FLinearColor::Red, float Duration = 3.f);

	UFUNCTION(BlueprintPure, Category = "GAS|Abilities")
	FGameplayTag GetAssignedSlotTag() const;

	UFUNCTION(BlueprintPure, Category = "GAS|Abilities")
	ACSBaseCharacter* GetCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "GAS|Abilities")
	UAnimMontage* GetMontageFromCharacter(FGameplayTag AbilityTag);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
	bool bDrawDebugs = false;

private:
	TWeakObjectPtr<ACSBaseCharacter> CachedBaseCharacter;
};
