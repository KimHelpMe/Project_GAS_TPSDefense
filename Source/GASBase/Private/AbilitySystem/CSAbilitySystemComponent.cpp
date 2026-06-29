// CSK


#include "AbilitySystem/CSAbilitySystemComponent.h"
#include "GameplayTags/CSGameplayTags.h"



void UCSAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	HandleAutoActivatedAbility(AbilitySpec);
}

void UCSAbilitySystemComponent::OnRep_ActivateAbilities()
{
	// 부모 RepNotify 처리 호출
	Super::OnRep_ActivateAbilities();

	// 어빌리티 목록 순회 중 추가/제거가 발생해도 안전하도록 스코프 잠금
	// 런타임에서 어빌리티는 동적으로 추가/제거될 수 있다
	// 글링 포인터 또는 배열 무효화 문제가 발생할 수 있다
	// FScopedAbilityListLock은 생성되는 시점부터 해당 스코프가 끝날 때까지 어빌리티 목록의 변경을 잠근다(실제 삭제를 지연시킨다)
	// RAII 패턴으로 동작하므로 스코프를 벗어나면 자동으로 해제된다
	FScopedAbilityListLock ActiveScopeLock(*this);

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		HandleAutoActivatedAbility(AbilitySpec);
	}
}

void UCSAbilitySystemComponent::HandleAutoActivatedAbility(const FGameplayAbilitySpec& AbilitySpec)
{
	// 어빌리티 CDO가 유효한지 먼저 확인
	// TObjectPtr<UGameplayAbility> 타입으로, 실제 인스턴스가 아닌 Class Default Object(CDO) 를 가리킨다
	// CDO는 해당 클래스의 기본값 템플릿으로 단 하나만 존재하며, 어빌리티 스펙들은 모두 이 CDO를 공유
	if (!IsValid(AbilitySpec.Ability)) return;


	// 어빌리티에 부여된 Asset Tag를 순회
	for (const FGameplayTag& Tag : AbilitySpec.Ability->GetAssetTags())
	{
		
		// MatchesTag는 부분 일치를 허용
		// MatchesTagExact는 완전 일치만 허용
		if (Tag.MatchesTagExact(CSGameplayTags::Abilities::ActivateOnGiven))
		{
			TryActivateAbility(AbilitySpec.Handle);
			return;

		}
	}
	

}
