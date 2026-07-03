// CSK


#include "AbilitySystem/CSAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "GameplayTags/CSGameplayTags.h"

// 어떤 프로퍼티를 리플리케이트할지, 그리고 리플리케이트될 때 어떤 조건/알림 방식을 쓸지 등록하는 함수
// 엔진이 게임 시작 시 한 번 호출해서 리플리케이션 테이블을 구성함
void UCSAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DOREPLIFETIME_CONDITION_NOTIFY: 조건부 리플리케이션 + OnRep 콜백 등록을 동시에 하는 매크로
	// COND_None: 별도 조건 없이 모든 클라이언트에 리플리케이트
	// REPNOTIFY_Always: 값이 실제로 바뀌지 않았어도 OnRep을 항상 호출
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxStamina, COND_None, REPNOTIFY_Always);

	/** REPNOTIFY_Always를 사용하는 이유
	 * 기본 동작: 값이 변경된 경우에만 RepNotify 호출
	 * 문제: GAS의 예측 시스템이 성공하면 서버 확정값과 CurrentValue가 일치 → RepNotify 누락
	 *       → BaseValue가 교정되지 않아 이후 버프/디버프 계산이 잘못됨
	 * 해결: REPNOTIFY_Always로 값이 같아도 항상 RepNotify를 호출해 BaseValue를 재교정
	 */


	 // 초기화 여부 플래그 복제 등록
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

	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	}

	// Health 어트리뷰트가 0 이하로 내려간 경우 → 대상 사망 처리
	if (Data.EvaluatedData.Attribute == GetHealthAttribute() && GetHealth() <= 0.f)
	{

		// 페이로드 생성 및 설정
		FGameplayEventData Payload;

		// 피격자의 아바타 액터를 Instigator로 설정
		Payload.Instigator = Data.Target.GetAvatarActor();

		// 이 게임플레이 이펙트를 발생시킨 주체(공격자) 액터 획득
		AActor* Causer = Data.EffectSpec.GetEffectContext().GetInstigator();

		// 공격자에게 Scored 이벤트 전송
		// ex) 경험치, 골드, 아이템 등
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Causer, CSGameplayTags::Events::Scored, Payload);
	}

	// 첫 번째 Effect가 적용될 때만 플래그 설정 및 브로드캐스트 (서버)
	if (!bAttributesInitialized)
	{
		bAttributesInitialized = true;

		// 서버 자신에게는 OnRep이 호출되지 않으므로 서버에서 필요한 로직이 있다면 여기서 직접 브로드캐스트해줘야 함
		// (클라이언트들은 bAttributesInitialized가 replicate된 뒤 OnRep_AttributesInitialized에서 브로드캐스트됨)
		OnAttributesInitialized.Broadcast();
	}
}

// 클라이언트에서 bAttributesInitialized 값이 리플리케이트되어 도착했을 때 호출됨
void UCSAttributeSet::OnRep_AttributesInitialized()
{
	// 복제된 플래그가 true이면 클라이언트에서도 브로드캐스트
	if (bAttributesInitialized)
	{
		OnAttributesInitialized.Broadcast();
	}
}

void UCSAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	// GAS 예측 시스템에 Rep Notify를 연결하는 필수 매크로
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldValue);
}

void UCSAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldValue);
}

void UCSAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Stamina, OldValue);
}

void UCSAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxStamina, OldValue);
}
