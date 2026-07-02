// CSK

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "AttributeSet.h" 

#include "CSWidgetComponent.generated.h"



class ACSBaseCharacter;
class UAbilitySystemComponent;
class UCSAbilitySystemComponent;
class UCSAttributeSet;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GASBASE_API UCSWidgetComponent : public UWidgetComponent
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

	// 어트리뷰트 변경 이벤트 바인딩
	UFUNCTION() //zzzzzzzzzzzzzzzzzzzzzㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋ 이거 안붙여서 2시간 날림 ㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋ
	void BindToAttributeChanges();

	// BindToAttributeChanges() 본문에서 반복될 로직을 별도 함수로 분리
	void BindWidgetToAttributeChanges(UWidget* WidgetObject, const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair);

	UFUNCTION()
	void OnASCInitialized(UAbilitySystemComponent* ASC, UAttributeSet* AS);

private:

	/** TWeakObjectPtr를 쓰는 이유
	* Widget Component는 ASC나 AttributeSet을 소유하지 않는다.
	* 이 객체들이 먼저 파괴될 경우 댕글링 포인터가 되는 것을 방지하기 위해 약한 참조(TWeakObjectPtr)를 사용한다
	*/

	// 소유권 없음 — 참조만 유지
	TWeakObjectPtr<ACSBaseCharacter> BaseCharacter;
	TWeakObjectPtr<UCSAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<UCSAttributeSet> AttributeSet;
};


