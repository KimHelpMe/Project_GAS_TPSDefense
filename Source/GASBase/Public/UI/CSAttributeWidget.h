// CSK

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystem/CSAttributeSet.h"


#include "CSAttributeWidget.generated.h"


UCLASS()
class GASBASE_API UCSAttributeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 주어진 pair가 이 위젯의 어트리뷰트 쌍과 일치하는지 확인 (Key = Attribute, Value = MaxAttribute)
	bool MatchesAttributes(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const;

	// 어트리뷰트 값이 변경되었을 때 호출 — AttributeSet에서 수치를 읽어 BP 이벤트 호출
	void OnAttributeChange(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair, const UCSAttributeSet* AttributeSet, float OldValue);

	// 위젯 블루프린트에서 구현할 이벤트 — 프로그레스바 업데이트 등 디자이너 담당
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Attribute Change"))
	void BP_OnAttributeChange(float NewValue, float NewMaxValue, float OldValue);

public:
	/** FGameplayAttribute란?
	* 어트리뷰트의 식별자다. 실제 수치 값을 갖고 있지 않으며, 어떤 어트리뷰트인지를 나타내는 구조체다.
	* 실제 수치는 반드시 AttributeSet을 통해 GetNumericValue(AttributeSet)으로 읽어야 한다.
	*/

	// 이 위젯이 담당할 어트리뷰트 (예: Health)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Attributes")
	FGameplayAttribute Attribute;

	// 이 위젯이 담당할 최대 어트리뷰트 (예: MaxHealth)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Attributes")
	FGameplayAttribute MaxAttribute;

};
