// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSWallSpawnButton.generated.h"

class USphereComponent;
class USceneComponent;
class ACSSpawnedWall;
class ACSPlayerCharacter;

// 플레이어가 밟으면 WallSpawnPoint 위치에 일시적인 차단 벽을 세우는 버튼.
// 밟은 뒤 CooldownDuration 동안은 다시 작동하지 않음.
// 비주얼(메시)은 블루프린트 자식(BP_WallSpawnButton)에서 추가.
UCLASS()
class GASBASE_API ACSWallSpawnButton : public AActor
{
	GENERATED_BODY()

public:
	ACSWallSpawnButton();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

private:
	void SpawnWall();
	void OnCooldownExpired();

public:
	// 스폰할 벽 클래스
	UPROPERTY(EditAnywhere, Category = "Wall")
	TSubclassOf<ACSSpawnedWall> WallClass;

	// 벽이 세워진 뒤 버튼이 다시 작동 가능해지기까지의 시간(초)
	UPROPERTY(EditAnywhere, Category = "Wall")
	float CooldownDuration = 60.f;

private:
	UPROPERTY(VisibleAnywhere, Category = "Wall")
	TObjectPtr<USphereComponent> OverlapVolume;

	// 벽이 스폰될 위치/회전 — 레벨에 배치한 버튼 인스턴스마다 자유롭게 조정 가능
	UPROPERTY(VisibleAnywhere, Category = "Wall")
	TObjectPtr<USceneComponent> WallSpawnPoint;

	bool bIsOnCooldown = false;

	FTimerHandle CooldownTimerHandle;
};
