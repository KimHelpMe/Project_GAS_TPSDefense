// CSK


#include "AbilitySystem/CSAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

// 어떤 프로퍼티를 리플리케이트할지, 그리고 리플리케이트될 때 어떤 조건/알림 방식을 쓸지 등록하는 함수
// 엔진이 게임 시작 시 한 번 호출해서 리플리케이션 테이블을 구성함
void UCSAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DOREPLIFETIME_CONDITION_NOTIFY: 조건부 리플리케이션 + OnRep 콜백 등록을 동시에 하는 매크로
	// COND_None: 별도 조건 없이 모든 클라이언트에 리플리케이트
	// REPNOTIFY_Always: 값이 실제로 바뀌지 않았어도(같은 값이 다시 와도) OnRep을 항상 호출

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxMana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME(ThisClass, bAttributesInitialized);
}

// 서버(Authority)에서 GameplayEffect가 적용되어 attribute 값이 실제로 변경된 "직후" 호출됨
// 클라이언트에서는 절대 호출되지 않음 (서버 전용 함수)
// 여기서 클램핑, 부가 로직(사망 처리, 초기화 완료 신호 등)을 처리하는 게 GAS 표준 패턴
void UCSAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}

	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}

	if (!bAttributesInitialized)
	{
		bAttributesInitialized = true;

		// 서버 자신에게는 OnRep이 호출되지 않으므로(로컬 변경은 OnRep 트리거 안 됨)
		// 서버에서 필요한 로직이 있다면 여기서 직접 브로드캐스트해줘야 함
		// (클라이언트들은 bAttributesInitialized가 replicate된 뒤 OnRep_AttributesInitialized에서 브로드캐스트됨)
		OnAttributesInitialized.Broadcast();
	}
}

// 클라이언트에서 bAttributesInitialized 값이 리플리케이트되어 도착했을 때 호출됨
void UCSAttributeSet::OnRep_AttributesInitialized()
{
	if (bAttributesInitialized)
	{
		OnAttributesInitialized.Broadcast();
	}
}

void UCSAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldValue);
}

void UCSAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldValue);
}

void UCSAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Mana, OldValue);
}

void UCSAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxMana, OldValue);
}
