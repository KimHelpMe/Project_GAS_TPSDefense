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

private:
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};
