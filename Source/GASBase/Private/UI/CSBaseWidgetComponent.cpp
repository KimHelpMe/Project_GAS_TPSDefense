// CSK

#include "UI/CSBaseWidgetComponent.h"
#include "GameObjects/CSBaseStructure.h"
#include "AbilitySystem/CSAttributeSet.h"
#include "AbilitySystem/CSAbilitySystemComponent.h"
#include "UI/CSAttributeWidget.h"
#include "Blueprint/WidgetTree.h"

void UCSBaseWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	// ASC, AttributeSet 포인터 획득 시도
	InitAbilitySystemData();

	if (!IsASCInitialized())
	{
		// ASC 미준비 → OnASCInitialized 델리게이트 대기
		if (BaseStructure.IsValid())
		{
			BaseStructure->OnASCInitialized.AddDynamic(this, &ThisClass::OnASCInitialized);
		}
		return;
	}

	// ASC 이미 준비됨 → 바로 어트리뷰트 델리게이트 단계로
	InitializeAttributeDelegate();
}

void UCSBaseWidgetComponent::InitAbilitySystemData()
{
	// Owner를 BaseStructure로 캐스트
	BaseStructure = Cast<ACSBaseStructure>(GetOwner());
	if (!BaseStructure.IsValid()) return;

	AttributeSet = Cast<UCSAttributeSet>(BaseStructure->GetAttributeSet());
	AbilitySystemComponent = Cast<UCSAbilitySystemComponent>(BaseStructure->GetAbilitySystemComponent());
}

bool UCSBaseWidgetComponent::IsASCInitialized() const
{
	return AbilitySystemComponent.IsValid() && AttributeSet.IsValid();
}

void UCSBaseWidgetComponent::InitializeAttributeDelegate()
{
	if (!AttributeSet->bAttributesInitialized)
	{
		// 아직 Effect가 적용되지 않았으면 델리게이트 대기
		AttributeSet->OnAttributesInitialized.AddDynamic(this, &ThisClass::BindToAttributeChanges);
	}
	else
	{
		// 이미 초기화 완료 → 바로 진행
		BindToAttributeChanges();
	}
}

void UCSBaseWidgetComponent::BindToAttributeChanges()
{
	// AttributeMap의 모든 어트리뷰트 쌍을 순회
	for (const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair : AttributeMap)
	{
		// 1) 위젯 컴포넌트가 직접 소유한 위젯 확인
		BindWidgetToAttributeChanges(GetUserWidgetObject(), Pair);

		// 2) 소유 위젯이 AttributeWidget이 아닐 경우 → 자식 위젯 전체 순회
		GetUserWidgetObject()->WidgetTree->ForEachWidget(
		[this, &Pair](UWidget* ChildWidget)
			{
				BindWidgetToAttributeChanges(ChildWidget, Pair);
			});
	}
}

void UCSBaseWidgetComponent::BindWidgetToAttributeChanges(UWidget* WidgetObject, const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair)
{
	// AttributeWidget이 아닌 위젯은 처리하지 않음
	UCSAttributeWidget* AttributeWidget = Cast<UCSAttributeWidget>(WidgetObject);
	if (!IsValid(AttributeWidget)) return;

	// 이 위젯이 담당하는 어트리뷰트 쌍이 아니면 무시
	if (!AttributeWidget->MatchesAttributes(Pair)) return;

	// 초기값 설정
	AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get(), 0.f);

	// 런타임 변경 구독 — ASC 델리게이트에 람다 바인딩
	TWeakObjectPtr<UCSAttributeWidget> WeakWidget(AttributeWidget);
	TTuple<FGameplayAttribute, FGameplayAttribute> CapturedPair = Pair;

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Key)
		.AddLambda([this, WeakWidget, CapturedPair]
		(const FOnAttributeChangeData& AttributeChangeData)
			{
				if (!WeakWidget.IsValid() || !AttributeSet.IsValid()) return;
				WeakWidget->OnAttributeChange(CapturedPair, AttributeSet.Get(), AttributeChangeData.OldValue);
			});
}

void UCSBaseWidgetComponent::OnASCInitialized(UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	AbilitySystemComponent = Cast<UCSAbilitySystemComponent>(ASC);
	AttributeSet = Cast<UCSAttributeSet>(AS);

	if (!IsASCInitialized()) return;
	InitializeAttributeDelegate(); // ASC 준비 완료 후 다음 단계
}
