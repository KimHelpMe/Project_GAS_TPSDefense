// CSK

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "AttributeSet.h"

#include "CSBaseWidgetComponent.generated.h"

class ACSBaseStructure;
class UAbilitySystemComponent;
class UCSAbilitySystemComponent;
class UCSAttributeSet;

// ACSBaseStructure(본진) 전용 HP바 위젯 바인딩 컴포넌트. 기존 UCSWidgetComponent는 ACSBaseCharacter에 하드코딩되어 있어
// Character가 아닌 ACSBaseStructure에는 그대로 쓸 수 없으므로 동일한 패턴으로 별도 구현.
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GASBASE_API UCSBaseWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// 에디터에서 등록할 어트리뷰트 쌍 목록 (Key = Attribute (예: Health), Value = MaxAttribute (예: MaxHealth))
	UPROPERTY(EditAnywhere)
	TMap<FGameplayAttribute, FGameplayAttribute> AttributeMap;

private:
	void InitAbilitySystemData();
	bool IsASCInitialized() const;

	// ASC·AttributeSet 유효성 확인 후 어트리뷰트 초기화 대기 처리
	void InitializeAttributeDelegate();

	UFUNCTION()
	void BindToAttributeChanges();

	// BindToAttributeChanges() 본문에서 반복될 로직을 별도 함수로 분리
	void BindWidgetToAttributeChanges(UWidget* WidgetObject, const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair);

	UFUNCTION()
	void OnASCInitialized(UAbilitySystemComponent* ASC, UAttributeSet* AS);

private:
	// 소유권 없음 — 참조만 유지
	TWeakObjectPtr<ACSBaseStructure> BaseStructure;
	TWeakObjectPtr<UCSAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<UCSAttributeSet> AttributeSet;
};
