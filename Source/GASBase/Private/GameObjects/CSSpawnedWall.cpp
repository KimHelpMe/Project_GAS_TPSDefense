// CSK

#include "GameObjects/CSSpawnedWall.h"
#include "Components/StaticMeshComponent.h"

ACSSpawnedWall::ACSSpawnedWall()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
	WallMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WallMesh->SetCollisionResponseToAllChannels(ECR_Block);
	// 내비메시가 이 액터를 장애물로 인식해서 적/아군 AI가 우회 경로를 탐색하도록 함
	WallMesh->SetCanEverAffectNavigation(true);
	SetRootComponent(WallMesh);
}

void ACSSpawnedWall::BeginPlay()
{
	Super::BeginPlay();

	// 파괴 타이밍은 서버가 결정하고 복제로 클라이언트에 전파됨
	if (!HasAuthority()) return;

	GetWorldTimerManager().SetTimer(LifetimeTimerHandle, this, &ThisClass::OnLifetimeExpired, WallLifetime, false);
}

void ACSSpawnedWall::OnLifetimeExpired()
{
	Destroy();
}
