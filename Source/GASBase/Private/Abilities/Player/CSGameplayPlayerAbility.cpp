// Stone


#include "Abilities/Player/CSGameplayPlayerAbility.h"
#include "Characters/CSPlayerCharacter.h"
#include "Player/CSPlayerController.h"

ACSPlayerCharacter* UCSGameplayPlayerAbility::GetCharacterFromActorInfo()
{
    if (!CachedCharacter.IsValid())
    {
        CachedCharacter = Cast<ACSPlayerCharacter>(CurrentActorInfo->AvatarActor);
    }
    return CachedCharacter.IsValid() ? CachedCharacter.Get() : nullptr;
}

ACSPlayerController* UCSGameplayPlayerAbility::GetPlayerControllerFromActorInfo()
{
    if (!CachedController.IsValid())
    {
        CachedController = Cast<ACSPlayerController>(CurrentActorInfo->PlayerController);
    }
    return CachedController.IsValid() ? CachedController.Get() : nullptr;
}

FVector UCSGameplayPlayerAbility::GetAimpoint()
{
    ACSPlayerController* PC = GetPlayerControllerFromActorInfo();
    if (!PC || !PC->PlayerCameraManager)
    {
        return FVector::ZeroVector;
    }

    const FVector Start = PC->PlayerCameraManager->GetCameraLocation();
    const FVector End = PC->PlayerCameraManager->GetCameraRotation().Vector() * TraceDistance;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetAvatarActorFromActorInfo());

    FHitResult Hit;
    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams);

    return bHit ? Hit.ImpactPoint : End;
}
