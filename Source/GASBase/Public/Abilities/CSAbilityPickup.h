#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSAbilityPickup.generated.h"

class UCSAbilityData;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class GASBASE_API ACSAbilityPickup : public AActor
{
	GENERATED_BODY()

public:
	ACSAbilityPickup();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

public:
	// 이 박스가 랜덤으로 뽑을 수 있는 스킬 후보군
	UPROPERTY(EditAnywhere, Category = "Pickup")
	TArray<TObjectPtr<UCSAbilityData>> PossibleAbilities;

private:

	// 실제 스폰박스 비주얼(메시, 파티클 등)은 블루프린트 자식(BP_AbilityPickup)에서.
	
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	TObjectPtr<USphereComponent> OverlapSphere;

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	TObjectPtr<UStaticMeshComponent> Mesh;
};