// CSK


#include "GameObjects/CSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayEffect.h"
#include "Characters/CSBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"

ACSProjectile::ACSProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 멀티플레이어 : 서버 -> 클라이언트 복제
	bReplicates = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetSphereRadius(10.f);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);   // 벽/바닥 → Hit
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 캐릭터 → Overlap
	CollisionComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);      // 카메라 무시
	CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);  // 라인트레이스 무시
	CollisionComponent->SetGenerateOverlapEvents(true);
	SetRootComponent(CollisionComponent);

	// Hit 이벤트 바인딩
	CollisionComponent->OnComponentHit.AddDynamic(this, &ACSProjectile::OnHit);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
	ProjectileMovementComponent->bShouldBounce = false;
}

void ACSProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (OtherActor == GetInstigator()) return;

	ACSBaseCharacter* BaseCharacter = Cast<ACSBaseCharacter>(OtherActor);
	if (!IsValid(BaseCharacter) || !BaseCharacter->IsAlive()) return;

	UAbilitySystemComponent* ASC = BaseCharacter->GetAbilitySystemComponent();
	if (!IsValid(ASC) || !HasAuthority()) return;

	// GE 적용
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddInstigator(GetInstigator(), this);

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffect, 1.f, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	SpawnImpactEffects();
	Destroy();
}

// 벽, 바닥 등
void ACSProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	SpawnImpactEffects();

	if (HasAuthority())
	{
		Destroy();
	}
}

