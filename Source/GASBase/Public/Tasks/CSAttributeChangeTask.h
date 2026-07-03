// CSK

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AttributeSet.h"  

#include "CSAttributeChangeTask.generated.h"

class UAbilitySystemComponent;
struct FOnAttributeChangeData;

// 어트리뷰트 변경 시 브로드캐스트할 델리게이트 타입
// 파라미터: 변경된 어트리뷰트, 새 값, 이전 값
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);


/**
BlueprintType — 이 클래스를 Blueprint 변수 타입으로 사용할 수 있게 한다.
ExposedAsyncProxy = AsyncTask : UBlueprintAsyncActionBase 기반 노드에서 
프록시 객체(태스크 인스턴스 자체)를 AsyncTask라는 이름의 출력 핀으로 노출한다.
*/

UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class GASBASE_API UCSAttributeChangeTask : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	// 블루프린트에서 구독할 수 있는 델리게이트 변수
	// → 블루프린트 노드의 추가 출력 실행 핀이 된다
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnAttributeChanged;

	// 블루프린트 노드를 생성하는 정적 팩토리 함수
	// Meta=BlueprintInternalUseOnly: 직접 호출하지 않고 내부 구현 전용
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UCSAttributeChangeTask* ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute);

	// 태스크 종료 및 리소스 정리용 함수
	UFUNCTION(BlueprintCallable)
	void EndTask();

private:
	TWeakObjectPtr<UAbilitySystemComponent> ASC;

	// 감시할 어트리뷰트 저장
	FGameplayAttribute AttributeToListenFor;

	// ASC 어트리뷰트 변경 델리게이트에 바인딩할 콜백
	void AttributeChanged(const FOnAttributeChangeData& Data);
};
