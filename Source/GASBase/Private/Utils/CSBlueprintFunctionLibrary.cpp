// CSK


#include "Utils/CSBlueprintFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/CSGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Data/CSAbilityDropSubsystem.h"
#include "Abilities/CSAbilityPickup.h"
#include "Abilities/CSAbilityData.h"


EHitDirection UCSBlueprintFunctionLibrary::GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator)
{
	// 내적: 두 벡터가 얼마나 같은 방향을 향하는지 (-1 ~ 1)
	const float Dot = FVector::DotProduct(TargetForward, ToInstigator);

	// 각도 120°~ 180° -> 공격자가 후방에 있음
	if (Dot < -0.5f)
	{
		return EHitDirection::Back;
	}

	// 각도 60°~ 120° -> Back도 Forward도 아님, 좌/우 판별 필
	if (Dot < 0.5f)
	{
		// 외적의 Z 성분 부호로 좌/우를 구분
		const FVector Cross = FVector::CrossProduct(TargetForward, ToInstigator);
		return Cross.Z < 0.f ? EHitDirection::Left : EHitDirection::Right;
	}

	// 각도 0°~ 60° -> 공격자가 전방에 있음
	return EHitDirection::Front;
}

FName UCSBlueprintFunctionLibrary::GetHitDirectionName(const EHitDirection& HitDirection)
{
	switch (HitDirection)
	{
	case EHitDirection::Left: 
		return FName("Left");

	case EHitDirection::Right: 
		return FName("Right");

	case EHitDirection::Front: 
		return FName("Front");

	case EHitDirection::Back: 
		return FName("Back");

	default: return FName("None");
	}
}

void UCSBlueprintFunctionLibrary::SendHitReactEventToActors(const TArray<AActor*>& ActorsHit, AActor* Instigator, const FGameplayTag& EventTag)
{
	UE_LOG(LogTemp, Warning, TEXT("SendHitReact - Instigator: %s, HasAuthority: %d"),
		*GetNameSafe(Instigator), IsValid(Instigator) ? (int)Instigator->HasAuthority() : -1);

	if (!IsValid(Instigator) || !Instigator->HasAuthority()) return;

	for (AActor* HitActor : ActorsHit)
	{
		if (!IsValid(HitActor)) continue;

		FGameplayEventData Payload;
		Payload.Instigator = Instigator;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, EventTag, Payload);
	}
}

void UCSBlueprintFunctionLibrary::SpawnFXAtLocation(const UObject* WorldContextObject,UFXSystemAsset* FXAsset,const FVector& Location, const FRotator& Rotation, const FVector& Scale)
{
	if (!IsValid(FXAsset)) return;

	// UNiagaraSystem으로 캬스트 성공하면 Niagara, 실패하면 Cascade
	if (UNiagaraSystem* NiagaraSystem = Cast<UNiagaraSystem>(FXAsset))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObject, NiagaraSystem, Location, Rotation, Scale);
	}
	else if (UParticleSystem* ParticleSystem = Cast<UParticleSystem>(FXAsset))
	{
		UGameplayStatics::SpawnEmitterAtLocation(WorldContextObject, ParticleSystem, Location, Rotation, Scale);
	}
}

FRotator UCSBlueprintFunctionLibrary::GetAimRotation(const FVector& MuzzleLocation, const FVector& AimPoint)
{
	const FVector Direction = (AimPoint - MuzzleLocation).GetSafeNormal();
	return Direction.Rotation();
}

FTransform UCSBlueprintFunctionLibrary::GetProjectileSpawnTransform(USceneComponent* MuzzleComponent, FName SocketName, const FVector& AimPoint, float ForwardOffset)
{
	if (!MuzzleComponent)
	{
		return FTransform::Identity;
	}

	const FVector MuzzleLocation = MuzzleComponent->GetSocketLocation(SocketName);
	const FRotator AimRotation = GetAimRotation(MuzzleLocation, AimPoint);

	// AimRotation 방향으로 ForwardOffset만큼 밀어낸 위치. ForwardOffset이 0이면 기존 동작과 동일.
	const FVector SpawnLocation = MuzzleLocation + (AimRotation.Vector() * ForwardOffset);

	return FTransform(AimRotation, SpawnLocation);
}

ACSAbilityPickup* UCSBlueprintFunctionLibrary::SpawnRandomSkillPickup(const UObject* WorldContextObject, TSubclassOf<ACSAbilityPickup> PickupClass, FVector SpawnLocation, int32 CurrentWave)
{
	if (!IsValid(WorldContextObject) || !IsValid(PickupClass)) return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;

	UGameInstance* GI = World->GetGameInstance();
	if (!IsValid(GI)) return nullptr;

	UCSAbilityDropSubsystem* DropSub = GI->GetSubsystem<UCSAbilityDropSubsystem>();
	if (!IsValid(DropSub)) return nullptr;

	UCSAbilityData* Rolled = DropSub->RollRandomAbility(CurrentWave);
	if (!IsValid(Rolled)) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ACSAbilityPickup* Pickup = World->SpawnActor<ACSAbilityPickup>(PickupClass, SpawnLocation, FRotator::ZeroRotator, Params);

	if (IsValid(Pickup))
	{
		Pickup->bUseDropTable = true;
		Pickup->SpawnWave = CurrentWave;
	}

	return Pickup;
}

bool UCSBlueprintFunctionLibrary::MontageHasRootMotion(UAnimMontage* Montage)
{
	return Montage ? Montage->HasRootMotion() : false;
}
