// CSK

#include "Characters/CSBaseCharacter.h"
#include "AbilitySystem/CSAbilitySystemComponent.h"
#include "Abilities/CSAbilityData.h"  
#include "Abilities/GameplayAbility.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


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

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bAlive, COND_None, REPNOTIFY_Always);
}

void ACSBaseCharacter::GiveStartupAbilities()
{
	// GAS의 어빌리티 부여 방식
	// - GAS에서 어빌리티를 부여할 때는 어빌리티 클래스(TSubclassOf)를 그대로 넘기지 않는다. 
	// - 대신 어빌리티 클래스를 기반으로 FGameplayAbilitySpec이라는 경량 구조체를 먼저 생성한 뒤, 이 스펙을 GiveAbility()에 전달한다. 
	// - 스펙에는 어빌리티 클래스 외에도 레벨, 입력 ID, 소스 오브젝트 등의 정보를 담을 수 있다. 
	// - 레벨을 따로 지정하지 않으면 기본값인 1로 설정된다.
	// - 이 타입의 헤더를 가져오기 위해 AbilitySystemComponent.h를 인클루드한다. (해당 헤더가 FGameplayAbilitySpec 헤더를 포함)

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;

	// 1) 고정 어빌리티(기본 공격 등)는 슬롯 시스템 없이 바로 부여
	for (const TSubclassOf<UGameplayAbility>& Ability : FixedStartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(Ability);
		ASC->GiveAbility(AbilitySpec);
	}

	// 2) Q/E/R/T 슬롯용 스킬만 TryPickupAbility로 부여 (슬롯 등록 필요)
	UCSAbilitySystemComponent* CSASC = Cast<UCSAbilitySystemComponent>(ASC);
	if (!IsValid(CSASC)) return;

	for (const TObjectPtr<UCSAbilityData>& AbilityData : StartupAbilityData)
	{
		CSASC->TryPickupAbility(AbilityData);
	}
}


// Gameplay Effect 적용은 서버에서만 수행한다. 복제(replication)는 GAS가 자동으로 처리하기 때문
// CSPlayerCharacter.cpp — PossessedBy() 안에서 호출
// CSEnemyCharacter.cpp — BeginPlay() 안에서 호출
void ACSBaseCharacter::InitializeAttributes() const
{
	// 일단 임시로 사용 ㅇㅇ... 나중에 checkf 방식으로 사용할 예정
	if (!IsValid(InitializeAttributesEffect)) return;
	checkf(IsValid(InitializeAttributesEffect), TEXT("InitializeAttributesEffect not set."));

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;

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

void ACSBaseCharacter::ApplyStartupEffects() const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;

	for (const TSubclassOf<UGameplayEffect>& Effect : StartupEffects)
	{
		if (!Effect) continue;
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Effect, 1.f, ContextHandle);
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void ACSBaseCharacter::ResetAttributes()
{
	if (!IsValid(ResetAttributesEffect)) return;
	checkf(IsValid(ResetAttributesEffect), TEXT("ResetAttributesEffect not set."));

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ResetAttributesEffect, 1.f, ContextHandle);

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

}

void ACSBaseCharacter::OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData)
{
	// Health가 0 이하로 내려가면 사망 처리
	//if (AttributeChangeData.NewValue <= 0.f)
	//{
	//	HandleDeath();
	//}

	 // 사망 판정은 GA_ListenForHealthChange → GA_Death 경로가 담당
	// 향후 피격 UI 갱신 등에 활용
}

void ACSBaseCharacter::HandleRespawn()
{
	bAlive = true;
}

void ACSBaseCharacter::OnRep_Alive()
{
}

void ACSBaseCharacter::HandleDeath_Implementation()
{
	bAlive = false;
}

