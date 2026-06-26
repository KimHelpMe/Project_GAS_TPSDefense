// CSK


#include "Player/CSPlayerState.h"
#include "AbilitySystem/CSAbilitySystemComponent.h"



ACSPlayerState::ACSPlayerState()
{
	// 기본값 1Hz는 어빌리티 동기화에 너무 느리므로 100Hz로 설정
	SetNetUpdateFrequency(100);

	// ASC 생성 및 복제 설정
	AbilitySystemComponent = CreateDefaultSubobject<UCSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// 복제 모드: Mixed (플레이어 컨트롤 캐릭터에 적합한 설정)
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

UAbilitySystemComponent* ACSPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
