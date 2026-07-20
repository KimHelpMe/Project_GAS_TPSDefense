

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CSCameraFadeComponent.generated.h"

class USpringArmComponent;

// 페이드 중인 액터 하나의 상태를 추적하는 구조체
USTRUCT()
struct FFadedActorInfo
{
	GENERATED_BODY()

	// 페이드 대상 액터
	TWeakObjectPtr<AActor> Actor;

	// 원본 머티리얼 배열 (복원용) - MeshComponent별로 저장
	TArray<TArray<TObjectPtr<UMaterialInterface>>> OriginalMaterials;

	// 생성한 Dynamic Material Instance 배열 - MeshComponent별로 저장
	TArray<TArray<TObjectPtr<UMaterialInstanceDynamic>>> DynamicMaterials;

	// 현재 Opacity 값 (1.0 = 불투명, 0.0 = 투명)
	float CurrentOpacity = 1.f;

	// 이번 프레임에 트레이스에 걸렸는지
	bool bIsBlocking = false;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GASBASE_API UCSCameraFadeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCSCameraFadeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	// 스피어 트레이스 실행 -> 차단 액터 목록 반환
	TArray<AActor*> SphereTrace() const;

	// 액터의 모든 StaticMeshCComponent에 DMI생성하고 원본을 저장
	void CreateDynamicMaterials(FFadedActorInfo& Info);

	// 액터의 머티리얼을 원본으로 복원
	void RestoreOriginalMaterials(FFadedActorInfo& Info);


private:
	// 현재 페이드하고 있는 액터 목록
	UPROPERTY()
	TMap<AActor*, FFadedActorInfo> FadedActors;

	// CameraBoom 캐싱 (트레이스 시작점)
	UPROPERTY()
	TObjectPtr<USpringArmComponent> CachedCameraBoom;

private:
	// 스피어 트레이스 반지름
	UPROPERTY(EditAnywhere, Category = "Camera Fade")
	float TraceRadius= 30.f;

	// 페이드 목표 Opacity 
	UPROPERTY(EditAnywhere, Category = "Camera Fade", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FadeOpacity = 0.15f;

	// 페이드 인/아웃 보간 속도
	UPROPERTY(EditAnywhere, Category = "Camera Fade")
	float FadeSpeed = 5.f;

	// DMI에서 조절할 Scalar Parameter 이름
	UPROPERTY(EditAnyWhere, Category = "Camera Fade")
	FName OpacityParameterName = TEXT("DitherOpacity");

	// 페이드 대상으로 인식할 액터 태그 
	UPROPERTY(EditAnyWhere, Category = "Camera Fade")
	FName FadeableTag = TEXT("Fadeable");

	// 페이드 트레이스에 사용할 콜리전 채널
	UPROPERTY(EditAnywhere, Category = "Camera Fade")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_GameTraceChannel1;
};
