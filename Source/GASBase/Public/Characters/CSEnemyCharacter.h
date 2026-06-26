// CSK

#pragma once

#include "CoreMinimal.h"
#include "Characters/CSBaseCharacter.h"
#include "CSEnemyCharacter.generated.h"

class UAbilitySystemComponent;

UCLASS()
class GASBASE_API ACSEnemyCharacter : public ACSBaseCharacter
{
	GENERATED_BODY()
	
public:
	ACSEnemyCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;


};
