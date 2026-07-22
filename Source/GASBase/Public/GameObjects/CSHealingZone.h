// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "CSHealingZone.generated.h"

class USphereComponent;
class ACSBaseCharacter;

// 안에 캐릭터가 머무는 동안 초당 일정량 체력을 회복시키는 존.
// 비주얼(메시/이펙트)은 블루프린트 자식(BP_HealingZone)에서 추가.
UCLASS()
class GASBASE_API ACSHealingZone : public AActor
{
	GENERATED_BODY()

public:
	ACSHealingZone();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	// 초당 회복량 — 레벨에 배치한 존 인스턴스마다 디테일 패널에서 조정 가능
	UPROPERTY(EditAnywhere, Category = "Heal")
	float HealPerSecond = 5.f;

	// 몬스터(적 캐릭터)도 회복 대상에 포함할지 여부 — 기본은 플레이어만 회복
	UPROPERTY(EditAnywhere, Category = "Heal")
	bool bCanHealEnemies = false;

	// 존 안에 있는 동안 재생할 GameplayCue (VFX/사운드는 GameplayCueNotify BP에서 처리, 자동 복제됨). 비워두면 재생 안 함.
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Cue", meta = (Categories = "GameplayCue"))
	FGameplayTag HealingCueTag;

private:
	UPROPERTY(VisibleAnywhere, Category = "Heal")
	TObjectPtr<USphereComponent> OverlapVolume;

	// 존 안에 있는 캐릭터별로 적용 중인 회복 GE 핸들 (여러 명이 동시에 들어올 수 있음)
	TMap<TWeakObjectPtr<ACSBaseCharacter>, FActiveGameplayEffectHandle> ActiveHealEffects;
};
