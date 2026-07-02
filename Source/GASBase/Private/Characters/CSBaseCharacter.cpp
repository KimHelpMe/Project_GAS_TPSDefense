// CSK

#include "Characters/CSBaseCharacter.h"
#include "AbilitySystem/CSAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Net/UnrealNetwork.h"

ACSBaseCharacter::ACSBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// 렌더링 여부와 관계없이 본 트랜스폼을 항상 갱신 (전용 서버에서 본 업데이트를 위해)
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

UAbilitySystemComponent* ACSBaseCharacter::GetAbilitySystemComponent() const
{
	// BaseCharacter는 ASC를 직접 소유하지 않음. 자식 클래스에서 오버라이드
	return nullptr;
}

void ACSBaseCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bAlive);
}

void ACSBaseCharacter::GiveStartupAbilities()
{
	// GAS의 어빌리티 부여 방식
	// - GAS에서 어빌리티를 부여할 때는 어빌리티 클래스(TSubclassOf)를 그대로 넘기지 않는다. 
	// - 대신 어빌리티 클래스를 기반으로 FGameplayAbilitySpec이라는 경량 구조체를 먼저 생성한 뒤, 이 스펙을 GiveAbility()에 전달한다. 
	// - 스펙에는 어빌리티 클래스 외에도 레벨, 입력 ID, 소스 오브젝트 등의 정보를 담을 수 있다. 
	// - 레벨을 따로 지정하지 않으면 기본값인 1로 설정된다.
	// - 이 타입의 헤더를 가져오기 위해 AbilitySystemComponent.h를 인클루드한다. (해당 헤더가 FGameplayAbilitySpec 헤더를 포함)

	if (!IsValid(GetAbilitySystemComponent())) return;

	for (const TSubclassOf<UGameplayAbility>& Ability : StartupAbilities)
	{
		// 어빌리티 클래스로 스펙 생성 (레벨 기본값 = 1)
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability);

		// ASC에 어빌리티 부여
		GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
	}
}


// Gameplay Effect 적용은 서버에서만 수행한다. 복제(replication)는 GAS가 자동으로 처리하기 때문
// CSPlayerCharacter.cpp — PossessedBy() 안에서 호출
// CSEnemyCharacter.cpp — BeginPlay() 안에서 호출
void ACSBaseCharacter::InitializeAttributes() const
{
	if (!IsValid(InitializeAttributesEffect)) return;

	checkf(IsValid(InitializeAttributesEffect), TEXT("InitializeAttributesEffect not set."));

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();

	// 1. Effect Context 생성
	// GameplayEffect를 적용할 때 "누가, 어떤 상황에서 이 효과를 발생시켰는지"에 대한 컨텍스트 생성
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();

	// 2. Effect Spec 생성
	// 실제 적용 가능한 GE Spec(설계도의 인스턴스)을 생성
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(InitializeAttributesEffect, 1.f, ContextHandle);

	// 3. 자신에게 GE 적용
	// 생성된 Spec을 자기 자신(이 캐릭터의 ASC)에게 즉시 적용
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void ACSBaseCharacter::HandleRespawn()
{
	bAlive = true;
}

void ACSBaseCharacter::HandleDeath()
{
	bAlive = false;
}

