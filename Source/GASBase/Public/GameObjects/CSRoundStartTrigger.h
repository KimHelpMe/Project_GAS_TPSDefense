// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSRoundStartTrigger.generated.h"

class USphereComponent;

// 플레이어가 처음 밟으면 지정된 라운드를 시작시키고, 이후로는 다시 작동하지 않는 트리거.
// 레벨에 배치해서 사용하며, EnemyManager(UCSEnemySpawnerComponent를 가진 액터)를 참조로 지정해야 함.
UCLASS()
class GASBASE_API ACSRoundStartTrigger : public AActor
{
	GENERATED_BODY()

public:
	ACSRoundStartTrigger();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

public:
	// 라운드를 시작시킬 EnemyManager 액터 (레벨의 BP_EnemyManager 인스턴스를 지정)
	UPROPERTY(EditInstanceOnly, Category = "Round")
	TObjectPtr<AActor> EnemyManager;

	// EnemyManager 블루프린트에 추가된, 첫 라운드를 시작시키는 커스텀 이벤트 이름
	UPROPERTY(EditAnywhere, Category = "Round")
	FName StartEventName = TEXT("StartFirstRound");

private:
	UPROPERTY(VisibleAnywhere, Category = "Round")
	TObjectPtr<USphereComponent> OverlapVolume;

	bool bHasTriggered = false;
};
