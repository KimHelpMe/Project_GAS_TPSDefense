// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSSpawnedWall.generated.h"

class UStaticMeshComponent;

// 버튼(ACSWallSpawnButton)이 스폰하는 일시적인 차단 벽. 일정 시간 뒤 자동으로 사라진다.
// 비주얼(메시)은 블루프린트 자식(BP_SpawnedWall)에서 추가.
UCLASS()
class GASBASE_API ACSSpawnedWall : public AActor
{
	GENERATED_BODY()

public:
	ACSSpawnedWall();

protected:
	virtual void BeginPlay() override;

private:
	void OnLifetimeExpired();

public:
	// 벽이 유지되는 시간(초) — 만료되면 자동으로 Destroy됨
	UPROPERTY(EditAnywhere, Category = "Wall")
	float WallLifetime = 15.f;

private:
	UPROPERTY(VisibleAnywhere, Category = "Wall")
	TObjectPtr<UStaticMeshComponent> WallMesh;

	FTimerHandle LifetimeTimerHandle;
};
