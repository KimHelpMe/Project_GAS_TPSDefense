// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"

#include "CSBaseCharacter.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayAbility;
class UGameplayEffect;

// // ASC와 AttributeSet이 초기화 완료됐을 때 브로드캐스트할 델리게이트 타입 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FASCInitialized, UAbilitySystemComponent*, ASC, UAttributeSet*, AS);


// Abstract
UCLASS(Abstract)
class GASBASE_API ACSBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACSBaseCharacter();

	//~ Begin IAbilitySystemInterface Interface
	// 자식 클래스(PlayerCharacter, EnemyCharacter)에서 실제 ASC를 반환하도록 오버라이드
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface Interface

	// 자식 클래스(PlayerCharacter, EnemyCharacter)에서 실제 AttributeSet를 반환하도록 오버라이드
	virtual UAttributeSet* GetAttributeSet() const { return nullptr; }

	//~ Begin AActor Interface
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End AActor Interface

	// ASC/AttributeSet 초기화 완료 시점을 알리는 델리게이트
	UPROPERTY(BlueprintAssignable)
	FASCInitialized OnASCInitialized;

protected:
	// StartupAbilities 배열에 있는 어빌리티들을 ASC에 부여(Give)하는 함수
	void GiveStartupAbilities();

	// InitializeAttributesEffect GE를 적용해서 Health/Mana 등 초기값 세팅하는 함수
	void InitializeAttributes() const;

	virtual void HandleRespawn();
	virtual void HandleDeath();
private:
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "GAS|Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Replicated)
	bool bAlive = true;
};
