// CSK

#pragma once

#include "CoreMinimal.h"
#include "Characters/CSBaseCharacter.h"
#include "CSEnemyCharacter.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

UCLASS()
class GASBASE_API ACSEnemyCharacter : public ACSBaseCharacter
{
	GENERATED_BODY()

public:
	ACSEnemyCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;
	virtual UAttributeSet* GetAttributeSet() const override;

	// 적 전용: Health ≤ 0 시 직접 HandleDeath 호출
	virtual void OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};
