// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "CSBaseStructure.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
// 본진(Base)이 파괴됐을 때 브로드캐스트되는 델리게이트 — GameMode/GameState가 구독해서 패배 처리에 사용
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBaseDestroyed);

// ASC/AttributeSet 초기화 완료 시점을 알리는 델리게이트 (멀티플레이어에서 위젯 등이 초기화 타이밍을 기다리기 위해 사용)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBaseASCInitialized, UAbilitySystemComponent*, ASC, UAttributeSet*, AS);

UCLASS()
class GASBASE_API ACSBaseStructure : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACSBaseStructure();

	//~ Begin IAbilitySystemInterface Interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface Interface

	//~ Begin AActor Interface
	// bDestroyed가 리플리케이션 대상이므로 GetLifetimeReplicatedProps()를 오버라이드
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End AActor Interface

	UPROPERTY(BlueprintAssignable, Category = "GAS|Death")
	FOnBaseDestroyed OnBaseDestroyed;

	// ASC/AttributeSet 초기화 완료 시점을 알리는 델리게이트 (CSBaseWidgetComponent가 구독)
	UPROPERTY(BlueprintAssignable, Category = "GAS")
	FOnBaseASCInitialized OnASCInitialized;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GAS|Death")
	void HandleBaseDestroyed();

	bool IsBaseDestroyed() const { return bDestroyed; }

	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
	virtual void BeginPlay() override;
	virtual void HandleBaseDestroyed_Implementation();

	// 어트리뷰트를 초기화하는 함수 - GE를 적용해서 Health/MaxHealth 초기값 세팅
	void InitializeAttributes() const;

	void OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData);

private:
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	// 어트리뷰트 초기화에 사용할 Gameplay Effect 클래스 (MaxHealth/Health 초기값)
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;

	// 파괴 시 실행할 GameplayCue 태그 (ImpactCueTag와 동일한 패턴 — VFX/사운드는 GameplayCueNotify BP에서 처리, 모든 클라이언트에 자동 복제됨)
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Cue", meta = (Categories = "GameplayCue"))
	FGameplayTag DestructionCueTag;

	UPROPERTY(BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	bool bDestroyed = false;
};
