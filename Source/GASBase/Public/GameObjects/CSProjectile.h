// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UGameplayEffect;

UCLASS()
class GASBASE_API ACSProjectile : public AActor
{
	GENERATED_BODY()

public:
	ACSProjectile();

	// 다른 액터가 이 액터와 겹쳤을 때 엔진이 자동으로 호출. 캐릭터 충돌 (Pawn 채널 → Overlap)
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	// 벽/바닥 충돌 (WorldStatic 채널 → Hit)
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SpawnImpactEffects(FVector Location, FRotator Rotation);

	// GE 적용·Destroy 직전에 충돌 이펙트를 스폰하는 지점을 블루프린트 측에 위임
	UFUNCTION(BlueprintImplementableEvent, Category = "GAS|Projectile")
	void SpawnImpactEffects();


protected:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "GAS|Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "GAS|Damage")
	TSubclassOf<UGameplayEffect> DamageEffect;

	// ExposeOnSpawn은 블루프린트의 SpawnActorFromClass 노드에 이 변수를 핀으로 노출
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Damage", meta = (ExposeOnSpawn = true, ClampMin = 0.0))
	float Damage = 10.f;


};
