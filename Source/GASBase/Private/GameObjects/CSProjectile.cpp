// CSK


#include "GameObjects/CSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayEffect.h"
#include "Characters/CSBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SphereComponent.h"
#include "Engine/OverlapResult.h"
#include "Utils/CSBlueprintFunctionLibrary.h"
#include "GameplayTags/CSGameplayTags.h"

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

	// GE 적용 (데미지)
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddInstigator(GetInstigator(), this);

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffect, AbilityLevel, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	// Cue 직접 실행 (GE와 분리)
	ExecuteImpactCue(GetActorLocation(), -GetVelocity().GetSafeNormal());
	
	// HitReact 이벤트 전송
	UCSBlueprintFunctionLibrary::SendHitReactEventToActors({ OtherActor }, GetInstigator(), CSGameplayTags::Events::HitReact);

	// BP 후킹 포인트 — 폭발형 투사체에서 SphereOverlapAtImpact 연결
	OnProjectileImpact(GetActorLocation());

	Destroy();
}

// 벽, 바닥 등 — GE 적용 대상이 없으므로 Instigator ASC를 통해 Cue를 직접 실행
void ACSProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority()) return;

	ExecuteImpactCue(Hit.ImpactPoint, Hit.ImpactNormal);

	// BP 후킹 포인트 — 폭발형 투사체에서 SphereOverlapAtImpact 연결
	OnProjectileImpact(Hit.ImpactPoint);
	Destroy();
}

void ACSProjectile::ExecuteImpactCue(const FVector& Location, const FVector& Normal)
{
	if (!ImpactCueTag.IsValid()) return;

	UAbilitySystemComponent* InstigatorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetInstigator());
	if (!IsValid(InstigatorASC)) return;

	FGameplayCueParameters CueParams;
	CueParams.Location = Location;
	CueParams.Normal = Normal;
	InstigatorASC->ExecuteGameplayCue(ImpactCueTag, CueParams);
}

void ACSProjectile::SphereOverlapAtImpact(const FVector& ImpactLocation, float Radius)
{
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetInstigator());

	GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		ImpactLocation,
		FQuat::Identity,
		ECC_Pawn,
		Sphere,
		QueryParams
	);

	TArray<AActor*> HitActors;
	for (const FOverlapResult& Result : OverlapResults)
	{
		ACSBaseCharacter* BaseCharacter = Cast<ACSBaseCharacter>(Result.GetActor());
		if (!IsValid(BaseCharacter) || !BaseCharacter->IsAlive()) continue;

		UAbilitySystemComponent* ASC = BaseCharacter->GetAbilitySystemComponent();
		if (!IsValid(ASC)) continue;

		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddInstigator(GetInstigator(), this);

		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffect, AbilityLevel, ContextHandle);
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		HitActors.Add(BaseCharacter);
	}

	// HitReact 일괄 전송
	UCSBlueprintFunctionLibrary::SendHitReactEventToActors(HitActors, GetInstigator(), CSGameplayTags::Events::HitReact);
}

