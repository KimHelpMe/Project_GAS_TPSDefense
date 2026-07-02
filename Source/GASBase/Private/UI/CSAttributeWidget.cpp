// CSK


#include "UI/CSAttributeWidget.h"

bool UCSAttributeWidget::MatchesAttributes(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const
{
    // Key = Attribute, Value = MaxAttribute 컨벤션으로 비교
    return Attribute == Pair.Key && MaxAttribute == Pair.Value;
}

void UCSAttributeWidget::OnAttributeChange(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair, const UCSAttributeSet* AttributeSet, float OldValue)
{
    // AttributeSet에서 실제 수치 읽기
    const float AttributeValue = Pair.Key.GetNumericValue(AttributeSet);
    const float MaxAttributeValue = Pair.Value.GetNumericValue(AttributeSet);

    // Blueprint 이벤트 호출 → 위젯 블루프린트가 비주얼 업데이트 담당
    BP_OnAttributeChange(AttributeValue, MaxAttributeValue, OldValue);
}
