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

class UCSAbilityData;

USTRUCT()
struct FCSSlotAbilityEntry
{
	GENERATED_BODY()

	// 어느 슬롯(Q/E/R/T)인지. TMap의 Key였던 걸 구조체 필드로 옮김
	// -> TMap은 UPROPERTY로 리플리케이트가 안 되기 때문에, 리플리케이트 가능한 TArray로 바꾸면서 필요해짐
	UPROPERTY()
	FGameplayTag SlotTag;

	UPROPERTY()
	FGameplayAbilitySpecHandle Handle;

	UPROPERTY()
	TObjectPtr<UCSAbilityData> AbilityData = nullptr;
};


UCLASS()
class GASBASE_API UCSAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// 서버: 어빌리티가 ASC에 부여될 때 호출
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;

	// 클라이언트: 활성화 어빌리티 목록이 리플리케이션될 때 호출되는 RepNotify
	virtual void OnRep_ActivateAbilities() override;

	// SlotToAbility(리플리케이트 프로퍼티)를 등록하기 위해 오버라이드
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 특정 어빌리티의 레벨을 지정한 값으로 "절대" 세팅
	UFUNCTION(BlueprintCallable, Category = "GAS|Abilities")
	void SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level);

	// 특정 어빌리티의 레벨을 기존 값에서 "상대적으로" 증감
	UFUNCTION(BlueprintCallable, Category = "GAS|Abilities")
	void AddToAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level = 1);

	// 드랍박스가 호출하는 단일 진입점. 성공(습득됨) 여부만 반환.
	UFUNCTION(BlueprintCallable, Category = "GAS|Abilities")
	bool TryPickupAbility(UCSAbilityData* AbilityData);

	// 슬롯으로 쓸 태그 목록 (Q/E/R/T). 캐릭터 기본값에서 지정.
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Abilities")
	TArray<FGameplayTag> SkillSlotTags;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotAbilityChanged, FGameplayTag, SlotTag, UCSAbilityData*, AbilityData);

	// 스킬 슬롯 UI가 아이콘 갱신을 위해 구독하는 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "GAS|Abilities")
	FOnSlotAbilityChanged OnSlotAbilityChanged;

	UFUNCTION(BlueprintCallable, Category = "GAS|Abilities")
	bool TryActivateAbilityBySlot(FGameplayTag SlotTag);

	// 슬롯에 배정된 어빌리티의 남은 쿨다운/전체 쿨다운을 조회 (스킬 슬롯 UI용)
	UFUNCTION(BlueprintCallable, Category = "GAS|Abilities")
	void GetSlotCooldown(FGameplayTag SlotTag, float& TimeRemaining, float& CooldownDuration);

	// UAbilitySystemComponent::AddLooseGameplayTag/RemoveLooseGameplayTag는 BlueprintCallable이 아니라서 블루프린트에서 직접 못 씀. Super로 기능 가져오고 Callable 추가함.
	UFUNCTION(BlueprintCallable, Category = "GAS|Tags")
	void AddLooseGameplayTag(FGameplayTag TagToAdd);

	UFUNCTION(BlueprintCallable, Category = "GAS|Tags")
	void RemoveLooseGameplayTag(FGameplayTag TagToRemove);

private:
	void HandleAutoActivatedAbility(const FGameplayAbilitySpec& AbilitySpec);

	// 클라이언트에서 SlotToAbility가 리플리케이트되어 도착했을 때 호출됨
	// 서버는 TryPickupAbility에서 직접 Broadcast하지만, 클라이언트는 이 함수를 통해서만
	// OnSlotAbilityChanged를 로컬로 발생시킬 수 있음 (Broadcast는 네트워크를 안 타므로)
	UFUNCTION()
	void OnRep_SlotToAbility();

	// Q/E/R/T 슬롯에 어떤 스킬이 배정됐는지 관리하는 목록.
	// 예전엔 TMap이었지만, TMap은 UPROPERTY(Replicated)로 클라이언트에 복제가 안 되기 때문에
	// TArray + 구조체 안의 SlotTag 필드 조합으로 대체함 (클라이언트 스킬 슬롯 UI 갱신을 위해 필요)
	UPROPERTY(ReplicatedUsing = OnRep_SlotToAbility)
	TArray<FCSSlotAbilityEntry> SlotToAbility;
};
