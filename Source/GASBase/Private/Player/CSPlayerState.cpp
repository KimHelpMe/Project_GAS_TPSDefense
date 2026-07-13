// CSK


#include "Player/CSPlayerState.h"
#include "AbilitySystem/CSAbilitySystemComponent.h"
#include "AbilitySystem/CSAttributeSet.h"
#include "Net/UnrealNetwork.h"

ACSPlayerState::ACSPlayerState()
{
	// 기본값 1Hz는 어빌리티 동기화에 너무 느리므로 100Hz로 설정
	SetNetUpdateFrequency(100);

	// ASC 생성 및 복제 설정
	AbilitySystemComponent = CreateDefaultSubobject<UCSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// 복제 모드: Mixed (플레이어 컨트롤 캐릭터에 적합한 설정)
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	/**  GAS의 자동 등록 메커니즘
	* CreateDefaultSubobject로 UAttributeSet 파생 오브젝트를 Outer(PlayerState)에 생성하면, 
	* GAS는 이 Outer에 붙어있는 ASC를 찾아 Attribute Set을 자동으로 등록한다. 
	* 개발자가 별도로 RegisterAttributeSet() 같은 함수를 호출할 필요가 없다.
	*/
	AttributeSet = CreateDefaultSubobject<UCSAttributeSet>(TEXT("AttributeSet"));
}

void ACSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, RespawnTime);
}

UAbilitySystemComponent* ACSPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
