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

FVector UCSGameplayPlayerAbility::GetAimpoint()
{
    return GetAimpointHitResult().ImpactPoint;
}

FHitResult UCSGameplayPlayerAbility::GetAimpointHitResult()
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

    GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams);

    // 아무것도 안 맞았을 때, 기존 GetAimpoint()와 동일하게 최대 사거리 지점을 ImpactPoint로 채워준다.
    // (bBlockingHit는 false로 남아있으니, 호출부에서 실제로 맞았는지 여부는 bBlockingHit로 구분하면 됨)
    if (!Hit.bBlockingHit)
    {
        Hit.ImpactPoint = End;
    }

    return Hit;
}
