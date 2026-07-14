#include "GameObjects/CSTargetActor_Teleport.h"
#include "Abilities/GameplayAbilityWorldReticle.h"
#include "CollisionQueryParams.h"

FHitResult ACSTargetActor_Teleport::PerformTrace(AActor* InSourceActor)
{
	FHitResult ReturnHitResult = Super::PerformTrace(InSourceActor);

	// 부모(SingleLineTrace)가 아무것도 못 맞췄으면(하늘을 보는 경우), 그 지점에서
	// 수직으로 아래로 트레이스를 한 번 더 쏴서 바닥을 찾는다.
	if (!ReturnHitResult.bBlockingHit && InSourceActor)
	{
		FCollisionQueryParams Params(SCENE_QUERY_STAT(CSTeleportGroundFallback), false);
		Params.AddIgnoredActor(InSourceActor);

		FVector DownStart = ReturnHitResult.Location;
		DownStart.Z = InSourceActor->GetActorLocation().Z + FallbackTraceStartHeight;

		FVector DownEnd = DownStart;
		DownEnd.Z -= FallbackTraceDownDistance;

		FHitResult GroundHit;
		if (GetWorld()->LineTraceSingleByProfile(GroundHit, DownStart, DownEnd, TraceProfile.Name, Params))
		{
			ReturnHitResult.Location = GroundHit.Location;
			ReturnHitResult.ImpactPoint = GroundHit.ImpactPoint;
			ReturnHitResult.bBlockingHit = true;
		}

		// Super가 이미 한 번 레티클 위치를 (하늘 쪽으로) 세팅해놨으니, 새로 계산한 값으로 다시 덮어쓴다.
		if (AGameplayAbilityWorldReticle* LocalReticleActor = ReticleActor.Get())
		{
			LocalReticleActor->SetActorLocation(ReturnHitResult.Location);
		}
	}

	return ReturnHitResult;
}