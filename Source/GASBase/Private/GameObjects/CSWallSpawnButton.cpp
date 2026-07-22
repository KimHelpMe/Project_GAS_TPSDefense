// CSK

#include "GameObjects/CSWallSpawnButton.h"
#include "Components/SphereComponent.h"
#include "GameObjects/CSSpawnedWall.h"
#include "Characters/CSPlayerCharacter.h"

ACSWallSpawnButton::ACSWallSpawnButton()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	OverlapVolume = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapVolume"));
	OverlapVolume->SetSphereRadius(100.f);
	OverlapVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetRootComponent(OverlapVolume);

	WallSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("WallSpawnPoint"));
	WallSpawnPoint->SetupAttachment(OverlapVolume);
}

void ACSWallSpawnButton::BeginPlay()
{
	Super::BeginPlay();

	OverlapVolume->OnComponentBeginOverlap.AddDynamic(this, &ACSWallSpawnButton::OnOverlapBegin);
}

void ACSWallSpawnButton::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 스폰/쿨다운 판정은 서버에서만 수행 (복제는 스폰된 액터가 자동 처리)
	if (!HasAuthority()) return;

	if (bIsOnCooldown) return;

	if (!IsValid(Cast<ACSPlayerCharacter>(OtherActor))) return;

	SpawnWall();
}

void ACSWallSpawnButton::SpawnWall()
{
	if (!IsValid(WallClass)) return;

	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	World->SpawnActor<ACSSpawnedWall>(WallClass, WallSpawnPoint->GetComponentTransform(), SpawnParams);

	bIsOnCooldown = true;
	GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &ThisClass::OnCooldownExpired, CooldownDuration, false);
}

void ACSWallSpawnButton::OnCooldownExpired()
{
	bIsOnCooldown = false;
}
