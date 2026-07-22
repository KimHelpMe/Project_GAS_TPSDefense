// CSK

#include "GameObjects/CSBaseStructure.h"
#include "AbilitySystem/CSAbilitySystemComponent.h"
#include "AbilitySystem/CSAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameplayTags/CSGameplayTags.h" 

ACSBaseStructure::ACSBaseStructure()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	// ASC를 컴포넌트로 생성
	AbilitySystemComponent = CreateDefaultSubobject<UCSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// 구조물이므로 Minimal 모드로 충분
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// EnemyCharacter와 동일하게 공용 AttributeSet 사용 (Stamina 등 미사용 어트리뷰트는 무시)
	AttributeSet = CreateDefaultSubobject<UCSAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ACSBaseStructure::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACSBaseStructure::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bDestroyed);
}

void ACSBaseStructure::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(AbilitySystemComponent)) return;

	// Owner와 Avatar 모두 자기 자신
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	// 초기화 완료 시점을 알리는 델리게이트 - 서버/클라이언트 양쪽에서 로컬로 브로드캐스트
	OnASCInitialized.Broadcast(AbilitySystemComponent, AttributeSet);

	// Gameplay Effect 적용 및 델리게이트 바인딩은 서버에서만 수행 (복제는 GAS가 자동 처리)
	if (!HasAuthority()) return;

	InitializeAttributes();

	UCSAttributeSet* CC_AttributeSet = Cast<UCSAttributeSet>(AttributeSet);
	if (!IsValid(CC_AttributeSet)) return;

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CC_AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);

	// 팀 설정
	AbilitySystemComponent->AddMinimalReplicationGameplayTag(CSGameplayTags::Team::Player);
}

void ACSBaseStructure::InitializeAttributes() const
{
	if (!IsValid(InitializeAttributesEffect)) return;

	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(InitializeAttributesEffect, 1.f, ContextHandle);

	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void ACSBaseStructure::OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData)
{
	if (bDestroyed) return;
	
	// Health가 0 이하로 내려가면 파괴 처리
	if (AttributeChangeData.NewValue <= 0.f)
	{
		HandleBaseDestroyed();
	}
}

void ACSBaseStructure::HandleBaseDestroyed_Implementation()
{
	bDestroyed = true;

	// Cue 실행 - ASC를 통해 실행하면 모든 클라이언트에 자동으로 복제됨 (ImpactCueTag와 동일한 패턴)
	if (DestructionCueTag.IsValid() && IsValid(AbilitySystemComponent))
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();
		AbilitySystemComponent->ExecuteGameplayCue(DestructionCueTag, CueParams);
	}

	OnBaseDestroyed.Broadcast();
}
