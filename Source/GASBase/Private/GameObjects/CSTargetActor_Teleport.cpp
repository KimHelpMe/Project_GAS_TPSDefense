#include "GameObjects/CSTargetActor_Teleport.h"
#include "Abilities/GameplayAbilityWorldReticle.h"
#include "CollisionQueryParams.h"

FHitResult ACSTargetActor_Teleport::PerformTrace(AActor* InSourceActor)
{
	FHitResult ForwardHit = Super::PerformTrace(InSourceActor);
	if (!InSourceActor)
	{
		return ForwardHit;
	}

	const FVector ActorLocation = InSourceActor->GetActorLocation();
	FVector Direction = (ForwardHit.TraceEnd - ForwardHit.TraceStart).GetSafeNormal();
	FVector FlatDirection = FVector(Direction.X, Direction.Y, 0.f).GetSafeNormal();

	const float HitDistance = FVector::Dist(ForwardHit.TraceStart, ForwardHit.ImpactPoint);
	const bool bUsePreciseHit = ForwardHit.bBlockingHit && HitDistance <= TeleportRange;

	FVector CandidateXY;
	if (bUsePreciseHit)
	{
		// ��ó�� �� �ִ� �鿡 �¾��� ���, �� ǥ�鿡�� ��� �������� ��¦ �о��
		// ���� �ٿ�Ʈ���̽��� �� ��ü�� �ɸ��� �ʵ��� ��
		CandidateXY = ForwardHit.ImpactPoint + ForwardHit.ImpactNormal * WallPushOffset;
	}
	else
	{
		CandidateXY = ActorLocation + FlatDirection * TeleportRange;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(CSTeleportGroundSnap), false);
	Params.AddIgnoredActor(InSourceActor);

	FVector DownStart = CandidateXY;
	DownStart.Z = ActorLocation.Z + FallbackTraceStartHeight;
	FVector DownEnd = DownStart;
	DownEnd.Z -= FallbackTraceDownDistance;

	FHitResult ReturnHitResult = ForwardHit;
	FHitResult GroundHit;
	if (GetWorld()->LineTraceSingleByProfile(GroundHit, DownStart, DownEnd, TraceProfile.Name, Params))
	{
		ReturnHitResult.Location = GroundHit.Location;
		ReturnHitResult.ImpactPoint = GroundHit.ImpactPoint;
		ReturnHitResult.bBlockingHit = true;
	}
	else
	{
		ReturnHitResult.bBlockingHit = false;
	}

	if (AGameplayAbilityWorldReticle* LocalReticleActor = ReticleActor.Get())
	{
		LocalReticleActor->SetActorLocation(ReturnHitResult.Location);
	}
	return ReturnHitResult;
}