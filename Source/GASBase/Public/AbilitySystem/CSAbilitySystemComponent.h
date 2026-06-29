// CSK

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CSAbilitySystemComponent.generated.h"

/**
 * 어빌리티가 부여될 때 자동으로 활성화되는 능력도 필요
 * UAbilitySystemComponent는 이 동작을 직접 지원하지 않기 때문에,
 * UAbilitySystemComponent 상속한 커스텀 클래스를 만들어 
 * OnGiveAbility 같은 가상 함수를 오버라이해서 구현
 *  어빌리티 부여·활성화, 게임플레이 이펙트 적용, 
 * 어트리뷰트 관리 등 GAS의 거의 모든 기능이 이 클래스를 통해 이루어진다. 
 * 커스텀 클래스를 만들어두면 이 동작들을 원하는 대로 오버라이드할 수 있다.
 */

UCLASS()
class GASBASE_API UCSAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()


public:
	// 서버: 어빌리티가 ASC에 부여될 때 호출
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;

	// 클라이언트: 활성화 어빌리티 목록이 리플리케이션될 때 호출되는 RepNotify
	virtual void OnRep_ActivateAbilities() override;

private:
	void HandleAutoActivatedAbility(const FGameplayAbilitySpec& AbilitySpec);
};
