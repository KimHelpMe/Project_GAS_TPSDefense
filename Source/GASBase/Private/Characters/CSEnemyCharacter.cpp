// CSK


#include "Characters/CSEnemyCharacter.h"
#include "AbilitySystem/CSAbilitySystemComponent.h"
#include "AbilitySystem/CSAttributeSet.h"


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

	/**
	* 별도의 Attribute Set 클래스를 만들 수도 있지만, UCSAttributeSet을 그대로 공유해도 된다.
	* 적이 스태미나를 사용하지 않더라도 해당 attribute를 그냥 무시하면 되며,
	* 빈 attribute 몇 개가 차지하는 리소스는 무시할 수 있는 수준이다.
	*/
	AttributeSet = CreateDefaultSubobject<UCSAttributeSet>(TEXT("AttributeSet"));

}

UAbilitySystemComponent* ACSEnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* ACSEnemyCharacter::GetAttributeSet() const
{
	return AttributeSet;
}

void ACSEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(GetAbilitySystemComponent())) return;
	
	// Owner와 Avatar 모두 자기 자신
	GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);

	// 능력치 초기화
	InitializeAttributes();

	// 초기화 완료 시점을 알리는 델리게이트
	// HasAuthority() 블록 바깥에서 호출하는 이유 -> 로컬, 서버 동시에 초기화하기 위해 
	OnASCInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());

	// 서버가 아닐 때는 나감요
	if (!HasAuthority()) return;

	// 어빌리티 부여는 서버에서만
	GiveStartupAbilities();


	UCSAttributeSet* CC_AttributeSet = Cast<UCSAttributeSet>(GetAttributeSet());
	if (!IsValid(CC_AttributeSet)) return;

	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(CC_AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
}

void ACSEnemyCharacter::OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData)
{
	if (AttributeChangeData.NewValue <= 0.f)
	{
		HandleDeath();
	}
}


