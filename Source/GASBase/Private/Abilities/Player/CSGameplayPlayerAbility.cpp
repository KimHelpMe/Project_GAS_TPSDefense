// Stone


#include "Abilities/Player/CSGameplayPlayerAbility.h"
#include "Characters/CSPlayerCharacter.h"
#include "Player/CSPlayerController.h"

ACSPlayerCharacter* UCSGameplayPlayerAbility::GetPlayerCharacterFromActorInfo()
{
    if (!CachedPlayerCharacter.IsValid())
    {
        CachedPlayerCharacter = Cast<ACSPlayerCharacter>(CurrentActorInfo->AvatarActor);
    }
    return CachedPlayerCharacter.IsValid() ? CachedPlayerCharacter.Get() : nullptr;
}

ACSPlayerController* UCSGameplayPlayerAbility::GetPlayerControllerFromActorInfo()
{
    if (!CachedPlayerController.IsValid())
    {
        CachedPlayerController = Cast<ACSPlayerController>(CurrentActorInfo->PlayerController);
    }
    return CachedPlayerController.IsValid() ? CachedPlayerController.Get() : nullptr;
}

FVector UCSGameplayPlayerAbility::GetAimpoint(bool bIgnorePawns)
{
    return GetAimpointHitResult(bIgnorePawns).ImpactPoint;
}

FHitResult UCSGameplayPlayerAbility::GetAimpointHitResult(bool bIgnorePawns)
{
    FHitResult Hit;

    ACSPlayerController* PC = GetPlayerControllerFromActorInfo();
    if (!PC || !PC->PlayerCameraManager)
    {
        return Hit;
    }

    const FVector Start = PC->PlayerCameraManager->GetCameraLocation();
    const FVector End = Start + PC->PlayerCameraManager->GetCameraRotation().Vector() * TraceDistance;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetAvatarActorFromActorInfo());

    if (bIgnorePawns)
    {
        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

        GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, FCollisionObjectQueryParams(ObjectTypes), QueryParams);

    }

    else 
    {
        GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel2, QueryParams);
    }
    
    // 아무것도 안 맞았을 때, 기존 GetAimpoint()와 동일하게 최대 사거리 지점을 ImpactPoint로 채워준다.
    // (bBlockingHit는 false로 남아있으니, 호출부에서 실제로 맞았는지 여부는 bBlockingHit로 구분하면 됨)
    if (!Hit.bBlockingHit)
    {
        Hit.ImpactPoint = End;
    }


    return Hit;
}

TArray<FHitResult> UCSGameplayPlayerAbility::GetSpreadHitResults(int32 PelletCount, float SpreadAngleDegrees)
{
    TArray<FHitResult> Results;

    ACSPlayerController* PC = GetPlayerControllerFromActorInfo();
    if (!PC || !PC->PlayerCameraManager)
    {
        return Results;
    }

    const FVector Start   = PC->PlayerCameraManager->GetCameraLocation();
    const FVector Forward = PC->PlayerCameraManager->GetCameraRotation().Vector();

    // 자기 자신은 트레이스에서 제외
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetAvatarActorFromActorInfo());

    for (int32 i = 0; i < PelletCount; i++)
    {
        // Forward를 중심으로 SpreadAngleDegrees 범위 안에서 랜덤 방향 생성
        // FMath::VRandCone은 라디안을 받으므로 DegreesToRadians 변환 필요
        const FVector Direction = FMath::VRandCone(
            Forward, FMath::DegreesToRadians(SpreadAngleDegrees));

        const FVector End = Start + Direction * TraceDistance;

        FHitResult Hit;
        GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel2, QueryParams);

        // GetAimpointHitResult()와 동일한 패턴:
        // 안 맞았을 때도 ImpactPoint를 최대 사거리 끝점으로 채워둔다 (VFX 끝점용)
        if (!Hit.bBlockingHit)
        {
            Hit.ImpactPoint = End;
        }

        Results.Add(Hit);
    }

    return Results;
}
