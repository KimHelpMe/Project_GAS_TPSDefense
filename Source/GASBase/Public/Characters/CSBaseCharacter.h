// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"  
#include "Delegates/DelegateCombinations.h"
#include "CSBaseCharacter.generated.h"


class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayAbility;
class UGameplayEffect;
class UCSAbilityData;

// 멀티플레이어에서는 ASC와 AttributeSet이 BeginPlay 시점에 아직 초기화되지 않았을 수 있다. 
// 이를 해결하기 위해 초기화 완료 시점을 알리는 델리게이트가 필요하다.

// ASC와 AttributeSet이 초기화 완료됐을 때 브로드캐스트할 델리게이트 타입 선언
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
	// bAlive가 리플리케이션 대상이므로 GetLifetimeReplicatedProps()를 오버라이드
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End AActor Interface

public:
	// ASC/AttributeSet 초기화 완료 시점을 알리는 델리게이트 (Blueprint에서도 바인딩할 수 있도록 BlueprintAssignable 지정)
	UPROPERTY(BlueprintAssignable)
	FASCInitialized OnASCInitialized;

	UFUNCTION(BlueprintCallable, Category = "GAS|Attributes")
	void ResetAttributes();

	UFUNCTION(BlueprintCallable, Category = "GAS|Death")
	virtual void HandleRespawn();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GAS|Death")
	void HandleDeath();


protected:
	virtual void HandleDeath_Implementation();

	// StartupAbilities 배열에 있는 어빌리티들을 ASC에 부여(Give)하는 함수
	void GiveStartupAbilities();

	// 어트리뷰트를 초기화하는 함수 - GE를 적용해서 Health/Mana 등 초기값 세팅
	void InitializeAttributes() const;

	void OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData);


public:
	bool IsAlive() const { return bAlive; }
	void SetAlive(bool bAliveStatus) { bAlive = bAliveStatus; }


private:
	// 항상 고정으로 시작하는 어빌리티 (좌클릭 기본 공격, 우클릭 등) — 슬롯 시스템과 무관
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> FixedStartupAbilities;

	// 드랍박스로 습득/업그레이드하는 Q/E/R/T 슬롯용 스킬
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Abilities")
	TArray<TObjectPtr<UCSAbilityData>> StartupAbilityData;

	// 어트리뷰트 초기화에 사용할 Gameplay Effect 클래스
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;

	UPROPERTY(EditDefaultsOnly, Category = "GAS|Effects")
	TSubclassOf<UGameplayEffect> ResetAttributesEffect;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Replicated)
	bool bAlive = true;
};
