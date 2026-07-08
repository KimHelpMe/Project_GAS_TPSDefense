// CSK

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "FXSystem.h"
#include "CSBlueprintFunctionLibrary.generated.h"

// 피격 방향을 나타내는 열거형 (블루프린트 노출을 위해 uint8 기반 필요)
UENUM(BlueprintType)
enum class EHitDirection : uint8
{
    Left,
    Right,
    Front,      // Forward 였으나, 파라곤 애니메이션 명이 Front라서 Front로 바꿈
    Back
};



UCLASS()
class GASBASE_API UCSBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
    // Dot Product / Cross Product로 피격 방향을 계산
    UFUNCTION(BlueprintPure)
    static EHitDirection GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator);

    // 계산된 방향을 몽타주 섹션 이름(FName)으로 변환
    UFUNCTION(BlueprintPure)
    static FName GetHitDirectionName(const EHitDirection& HitDirection);

    // 피격 대상 배열에 HitReact 게임플레이 이벤트를 전송한다.
    // 서버에서만 호출할 것. Instigator는 공격을 유발한 액터(공격자).
    UFUNCTION(BlueprintCallable, Category = "GAS|Abilities")
    static void SendHitReactEventToActors(const TArray<AActor*>& ActorsHit, AActor* Instigator, const FGameplayTag& EventTag);

    // Cascade/Niagara 자동 분기하여 지정 위치에 FX를 스폰한다.
    // UFXSystemAsset은 UParticleSystem(Cascade)과 UNiagaraSystem의 공통 베이스라 둘 다 받을 수 있다.
    UFUNCTION(BlueprintCallable, Category = "GAS|FX", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
    static void SpawnFXAtLocation(
        const UObject* WorldContextObject,
        UFXSystemAsset* FXAsset,
        const FVector& Location,
        const FRotator& Rotation,
        const FVector& Scale
    );

    UFUNCTION(BlueprintPure, Category = "GAS|Aiming")
    static FRotator GetAimRotation(const FVector& MuzzleLocation, const FVector& AimPoint);

    // ForwardOffset: AimRotation 방향(Rotation.Vector())으로 스폰 위치를 얼마나 밀어낼지. 기본값 0이면 기존과 동일하게 소켓 위치 그대로 반환.
    UFUNCTION(BlueprintPure, Category = "GAS|Aiming")
    static FTransform GetProjectileSpawnTransform(USceneComponent* MuzzleComponent, FName SocketName, const FVector& AimPoint, float ForwardOffset = 0.f);
};
