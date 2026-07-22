// CSK

#include "GameObjects/CSRoundStartTrigger.h"
#include "Components/SphereComponent.h"
#include "Characters/CSPlayerCharacter.h"

ACSRoundStartTrigger::ACSRoundStartTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	OverlapVolume = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapVolume"));
	OverlapVolume->SetSphereRadius(150.f);
	OverlapVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetRootComponent(OverlapVolume);
}

void ACSRoundStartTrigger::BeginPlay()
{
	Super::BeginPlay();

	OverlapVolume->OnComponentBeginOverlap.AddDynamic(this, &ACSRoundStartTrigger::OnOverlapBegin);
}

void ACSRoundStartTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 발동 판정은 서버에서만 수행 (StartRound가 스폰하는 적은 복제된 액터 스폰이라 서버에서만 실행되어야 함)
	if (!HasAuthority()) return;

	if (bHasTriggered) return;

	if (!IsValid(Cast<ACSPlayerCharacter>(OtherActor))) return;

	if (!IsValid(EnemyManager)) return;

	// EnemyManager 블루프린트에 정의된 커스텀 이벤트(StartEventName)를 이름으로 찾아 호출
	// (BP_EnemyManager는 순수 블루프린트라 C++에서 직접 함수를 호출할 수 없어 리플렉션으로 호출)
	UFunction* StartFunction = EnemyManager->FindFunction(StartEventName);
	if (!StartFunction) return;

	bHasTriggered = true;
	EnemyManager->ProcessEvent(StartFunction, nullptr);
}
