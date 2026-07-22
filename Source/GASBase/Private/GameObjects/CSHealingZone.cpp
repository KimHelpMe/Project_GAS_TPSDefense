// CSK

#include "GameObjects/CSHealingZone.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"
#include "Characters/CSBaseCharacter.h"
#include "Characters/CSEnemyCharacter.h"
#include "AbilitySystem/CSGE_Heal.h"
#include "GameplayTags/CSGameplayTags.h"

ACSHealingZone::ACSHealingZone()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	OverlapVolume = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapVolume"));
	OverlapVolume->SetSphereRadius(200.f);
	OverlapVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetRootComponent(OverlapVolume);
}

void ACSHealingZone::BeginPlay()
{
	Super::BeginPlay();

	OverlapVolume->OnComponentBeginOverlap.AddDynamic(this, &ACSHealingZone::OnOverlapBegin);
	OverlapVolume->OnComponentEndOverlap.AddDynamic(this, &ACSHealingZone::OnOverlapEnd);
}

void ACSHealingZone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 존이 파괴/제거될 때 안에 남아있던 캐릭터들의 회복 효과도 함께 정리
	for (const TPair<TWeakObjectPtr<ACSBaseCharacter>, FActiveGameplayEffectHandle>& Pair : ActiveHealEffects)
	{
		if (ACSBaseCharacter* Character = Pair.Key.Get())
		{
			if (UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent())
			{
				ASC->RemoveActiveGameplayEffect(Pair.Value);

				if (HealingCueTag.IsValid())
				{
					ASC->RemoveGameplayCue(HealingCueTag);
				}
			}
		}
	}
	ActiveHealEffects.Empty();

	Super::EndPlay(EndPlayReason);
}

void ACSHealingZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// GE 적용은 서버에서만 수행 (복제는 GAS가 자동 처리)
	if (!HasAuthority()) return;

	ACSBaseCharacter* Character = Cast<ACSBaseCharacter>(OtherActor);
	if (!IsValid(Character) || !Character->IsAlive()) return;

	if (!bCanHealEnemies && Character->IsA<ACSEnemyCharacter>()) return;

	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(UCSGE_Heal::StaticClass(), 1.f, ContextHandle);
	SpecHandle.Data->SetSetByCallerMagnitude(CSGameplayTags::Data::Heal_Amount, HealPerSecond * UCSGE_Heal::TickInterval);

	const FActiveGameplayEffectHandle ActiveHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	ActiveHealEffects.Add(Character, ActiveHandle);

	if (HealingCueTag.IsValid())
	{
		ASC->AddGameplayCue(HealingCueTag);
	}
}

void ACSHealingZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;

	ACSBaseCharacter* Character = Cast<ACSBaseCharacter>(OtherActor);
	if (!IsValid(Character)) return;

	if (const FActiveGameplayEffectHandle* Handle = ActiveHealEffects.Find(Character))
	{
		if (UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent())
		{
			ASC->RemoveActiveGameplayEffect(*Handle);

			if (HealingCueTag.IsValid())
			{
				ASC->RemoveGameplayCue(HealingCueTag);
			}
		}
		ActiveHealEffects.Remove(Character);
	}
}
