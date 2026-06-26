// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"

#include "CSBaseCharacter.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;

UCLASS(Abstract)
class GASBASE_API ACSBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACSBaseCharacter();

	//~ Begin IAbilitySystemInterface Interface
	
	// 베이스 클래스는 ASC를 가지지 않으므로 nullptr 반환
	// 자식 클래스(PlayerCharacter, EnemyCharacter)에서 실제 ASC를 반환하도록 오버라이드
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//~ End IAbilitySystemInterface Interface

protected:
	void GiveStartupAbilities();

private:
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};
