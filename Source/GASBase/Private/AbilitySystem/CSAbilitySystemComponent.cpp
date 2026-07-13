// CSK


#include "AbilitySystem/CSAbilitySystemComponent.h"
#include "Abilities/CSAbilityData.h"
#include "GameplayTags/CSGameplayTags.h"
#include "Net/UnrealNetwork.h"



void UCSAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	HandleAutoActivatedAbility(AbilitySpec);
}

void UCSAbilitySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 서버에서 정해진 슬롯 배정 정보를 클라이언트에도 복제
	DOREPLIFETIME(UCSAbilitySystemComponent, SlotToAbility);
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

void UCSAbilitySystemComponent::SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level)
{
	// Ability Spec은 클라에서 직접 수정하면 안 됨
	if (!IsValid(GetAvatarActor()) || !GetAvatarActor()->HasAuthority()) return;

	// ASC의 ActivatableAbilities 목록에서 찾음
	// (이미 GiveAbility로 부여되어 있어야 찾아짐 없으면 nullptr)
	if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass))
	{
		// 레벨을 지정한 값으로 설정
		AbilitySpec->Level = Level;

		// Spec이 변경됐음을 표시 -> 리플리케이션 트리거
		// FGameplayAbilitySpec은 값만 바꾼다고 자동으로 클라에 리플리케이트 안 됨cs
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UCSAbilitySystemComponent::AddToAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level)
{
	// 서버(Authority)에서만 레벨 변경 허용
	if (!IsValid(GetAvatarActor()) || !GetAvatarActor()->HasAuthority()) return;

	if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass))
	{
		// 현재 레벨에 지정한 값만큼 누적 증가
		AbilitySpec->Level += Level;

		// 변경 사항을 즉시 복제 트리거
		MarkAbilitySpecDirty(*AbilitySpec);
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




bool UCSAbilitySystemComponent::TryPickupAbility(UCSAbilityData* AbilityData)
{
	if (!IsValid(AbilityData) || !AbilityData->AbilityClass) return false;

	// 이미 같은 스킬을 보유 중이면 → 업그레이드
	for (const FCSSlotAbilityEntry& Entry : SlotToAbility)
	{
		if (Entry.AbilityData && Entry.AbilityData->AbilityClass == AbilityData->AbilityClass)
		{
			AddToAbilityLevel(AbilityData->AbilityClass, 1);
			return true;
		}
	}

	// 빈 슬롯 탐색
	for (const FGameplayTag& SlotTag : SkillSlotTags)
	{
		// 이 SlotTag를 이미 쓰고 있는 항목이 있는지 확인 (TMap::Contains 대체)
		const bool bSlotTaken = SlotToAbility.ContainsByPredicate(
			[&SlotTag](const FCSSlotAbilityEntry& Entry) { return Entry.SlotTag == SlotTag; });

		if (!bSlotTaken)
		{
			FGameplayAbilitySpec NewSpec(AbilityData->AbilityClass, AbilityData->AbilityLevel);
			NewSpec.GetDynamicSpecSourceTags().AddTag(SlotTag); // 슬롯 부여 시 슬롯 태그 기록.
			const FGameplayAbilitySpecHandle NewHandle = GiveAbility(NewSpec);

			SlotToAbility.Add(FCSSlotAbilityEntry{ SlotTag, NewHandle, AbilityData });

			// 서버(또는 리슨 서버의 로컬 클라이언트) 자신의 UI를 위한 즉시 브로드캐스트.
			// 원격 클라이언트는 이 Broadcast를 받지 못하므로, SlotToAbility가 리플리케이트된 뒤
			// OnRep_SlotToAbility에서 별도로 다시 브로드캐스트해줘야 함
			OnSlotAbilityChanged.Broadcast(SlotTag, AbilityData);
			return true;
		}
	}

	// 슬롯 꽉 참 + 미보유 스킬 → 습득 실패
	return false;
}

bool UCSAbilitySystemComponent::TryActivateAbilityBySlot(FGameplayTag SlotTag)
{
	const FCSSlotAbilityEntry* Entry = SlotToAbility.FindByPredicate(
		[&SlotTag](const FCSSlotAbilityEntry& Entry) 
		{ 
			return Entry.SlotTag == SlotTag; 
		});

	if (!Entry) return false;

	// 디버그: 레벨 출력
	if (const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Entry->Handle))
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Level: %d"), *SlotTag.ToString(), Spec->Level);
	}

	return TryActivateAbility(Entry->Handle);
}

void UCSAbilitySystemComponent::GetSlotCooldown(FGameplayTag SlotTag, float& TimeRemaining, float& CooldownDuration)
{
	TimeRemaining = 0.f;
	CooldownDuration = 0.f;

	const FCSSlotAbilityEntry* Entry = SlotToAbility.FindByPredicate(
		[&SlotTag](const FCSSlotAbilityEntry& Entry) { return Entry.SlotTag == SlotTag; });

	if (!Entry || !Entry->Handle.IsValid()) return;

	const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Entry->Handle);
	if (!Spec || !Spec->Ability) return;

	Spec->Ability->GetCooldownTimeRemainingAndDuration(Entry->Handle, AbilityActorInfo.Get(), TimeRemaining, CooldownDuration);
}

void UCSAbilitySystemComponent::AddLooseGameplayTag(FGameplayTag TagToAdd)
{
	if (!TagToAdd.IsValid()) return;

	// AddLooseGameplayTag는 서버 자신에게만 적용되고 클라이언트에는 복제되지 않는다.
	// 그래서 클라이언트 예측(Local Predicted) 어빌리티가 이 태그 상태를 보지 못해
	// 잘못 예측 실행되는 문제가 있었음 -> MinimalReplicationTags를 사용하는,
	// 실제로 클라이언트에 복제되는 버전으로 교체
	Super::AddMinimalReplicationGameplayTag(TagToAdd);
}

void UCSAbilitySystemComponent::RemoveLooseGameplayTag(FGameplayTag TagToRemove)
{
	if (!TagToRemove.IsValid()) return;
	Super::RemoveMinimalReplicationGameplayTag(TagToRemove);
}

void UCSAbilitySystemComponent::OnRep_SlotToAbility()
{
	// 클라이언트에 SlotToAbility 전체가 새로 도착했을 때 호출됨.
	// 정확히 "무엇이 바뀌었는지" 추적하지 않고, 매번 전체를 다시 브로드캐스트한다.
	// (스킬 슬롯 UI 쪽에서 자기 SlotTag와 일치하는 것만 반영하므로 중복 브로드캐스트는 문제 없음)
	for (const FCSSlotAbilityEntry& Entry : SlotToAbility)
	{
		OnSlotAbilityChanged.Broadcast(Entry.SlotTag, Entry.AbilityData);
	}
}