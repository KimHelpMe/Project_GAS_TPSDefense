// CSK


#include "Characters/CSEnemyCharacter.h"
#include "AbilitySystem/CSAbilitySystemComponent.h"


ACSEnemyCharacter::ACSEnemyCharacter()
{
	// 적 캐릭터는 Tick이 필요 없음
	PrimaryActorTick.bCanEverTick = false;

	// ASC를 컴포넌트로 생성
	AbilitySystemComponent = CreateDefaultSubobject<UCSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	// 멀티플레이어 복제 활성화
	AbilitySystemComponent->SetIsReplicated(true);

	// 적에게는 Minimal 모드가 적합
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

UAbilitySystemComponent* ACSEnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACSEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(AbilitySystemComponent)) return;
	
	// Owner와 Avatar 모두 자기 자신
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	// 서버가 아닐 때는 나감요
	if (!HasAuthority()) return;

	// 어빌리티 부여는 서버에서만
	GiveStartupAbilities();

}
