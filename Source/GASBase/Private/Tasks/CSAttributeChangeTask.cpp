// CSK


#include "Tasks/CSAttributeChangeTask.h"
#include "AbilitySystemComponent.h"

UCSAttributeChangeTask* UCSAttributeChangeTask::ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute)
{
    // 태스크 인스턴스 생성
    UCSAttributeChangeTask* WaitForAttributeChangeTask = NewObject<UCSAttributeChangeTask>();

    // 멤버 변수 초기화
    WaitForAttributeChangeTask->ASC = AbilitySystemComponent;
    WaitForAttributeChangeTask->AttributeToListenFor = Attribute;


    // ASC가 유효하지 않아 태스크가 필요 없어진 경우, RemoveFromRoot()를 명시적으로 호출
    if (!IsValid(AbilitySystemComponent))
    {
        WaitForAttributeChangeTask->RemoveFromRoot();
        return nullptr;
    }


    // ASC의 어트리뷰트 변경 델리게이트에 콜백 바인딩
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(WaitForAttributeChangeTask, &UCSAttributeChangeTask::AttributeChanged);

    return WaitForAttributeChangeTask;
}

void UCSAttributeChangeTask::AttributeChanged(const FOnAttributeChangeData& Data)
{
    // 어트리뷰트 값이 바뀔 때마다 델리게이트 브로드캐스트
    // → 블루프린트 노드의 추가 출력 실행 핀 트리거
    OnAttributeChanged.Broadcast(Data.Attribute, Data.NewValue, Data.OldValue);
}

void UCSAttributeChangeTask::EndTask()
{
    if (ASC.IsValid())
    {
        ASC->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this);
    }


    // SetReadyToDestroy()는 UBlueprintAsyncActionBase에 정의된 함수로, 이 태스크가 더 이상 필요하지 않음을 엔진에 알린다.
    // MarkAsGarbage()는 UObject를 GC 수거 대상으로 명시적으로 표시한다.
    // 두 함수를 함께 호출하는 것이 이 태스크 계열의 표준 정리 패턴이다
    SetReadyToDestroy();
    MarkAsGarbage();
}