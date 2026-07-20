


#include "Camera/CSCameraFadeComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "Materials/MaterialInstanceDynamic.h"


UCSCameraFadeComponent::UCSCameraFadeComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	// 카메라는 로컬 전용이므로 리플리케이션 불필요
	SetIsReplicatedByDefault(false);
}


void UCSCameraFadeComponent::BeginPlay()
{
	Super::BeginPlay();

	// 로컬 컨트롤 캐릭터가 아니면 틱 비활성화 
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!IsValid(OwnerPawn) || !OwnerPawn->IsLocallyControlled())
	{
		SetComponentTickEnabled(false);
		return;
	}

	// Spring Arm 캐싱
	CachedCameraBoom = OwnerPawn->FindComponentByClass<USpringArmComponent>();
}

void UCSCameraFadeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 기존 추적 액터들 전부 false로 초기화
	for (TPair<AActor*, FFadedActorInfo>& Pair : FadedActors)
	{
		Pair.Value.bIsBlocking = false;
	}

	// 스피어 트레이스로 현재 차단 중인 액터 목록 수집
	TArray<AActor*> BlockingActors = SphereTrace();

	// Hit된 액터 처리 - 신규면 DMI 생성, 기존이면 차단 유지 표시
	for (AActor* Actor : BlockingActors)
	{
		if (FFadedActorInfo* Existing = FadedActors.Find(Actor))
		{
			Existing->bIsBlocking = true;
		}
		else
		{
			FFadedActorInfo NewInfo;
			NewInfo.Actor = Actor;
			NewInfo.bIsBlocking = true;
			NewInfo.CurrentOpacity = 1.f;
			CreateDynamicMaterials(NewInfo);
			FadedActors.Add(Actor, MoveTemp(NewInfo));
		}
	}

	// Opacity 보간 + 복원 완료 판정
	TArray<AActor*> ActorsToRemove;

	for (TPair<AActor*, FFadedActorInfo>& Pair : FadedActors)
	{
		FFadedActorInfo& Info = Pair.Value;

		// 차단 중이면 목표 Opacity 내리고, 아니면 1.0으로 복원
		const float TargetOpacity = Info.bIsBlocking ? FadeOpacity : 1.f;

		// FInterpTo: 현재 목표값으로 DeltaTime * FadeSpeed 비율로 부드럽게 이동
		Info.CurrentOpacity = FMath::FInterpTo(Info.CurrentOpacity, TargetOpacity, DeltaTime, FadeSpeed);

		// 모든 DMI에 현재 Opacity 적용
		for (const TArray<TObjectPtr<UMaterialInstanceDynamic>>& MeshMaterials : Info.DynamicMaterials)
		{
			for (UMaterialInstanceDynamic* MaterialInstanceDynamic : MeshMaterials)
			{
				if (IsValid(MaterialInstanceDynamic))
				{
					MaterialInstanceDynamic->SetScalarParameterValue(OpacityParameterName, Info.CurrentOpacity);
				}
			}
		}

		// 차단 해제 + Opacity가 거의 1.0 -> 완전 복원 완료
		if (!Info.bIsBlocking && FMath::IsNearlyEqual(Info.CurrentOpacity, 1.f, 0.01f))
		{
			RestoreOriginalMaterials(Info);
			ActorsToRemove.Add(Pair.Key);
		}
	}

	// 복원된 액터를 맵에서 제거
	for (AActor* Actor : ActorsToRemove)
	{
		FadedActors.Remove(Actor);
	}
}


TArray<AActor*> UCSCameraFadeComponent::SphereTrace() const
{
	TArray<AActor*> Result;

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!IsValid(Character)) return Result;

	if (!IsValid(CachedCameraBoom)) return Result;

	// 실제 카메라 위치 ~ 캐릭터 위치
	const FVector Start = CachedCameraBoom->GetComponentLocation() + CachedCameraBoom->GetForwardVector() * (-CachedCameraBoom->TargetArmLength);
	const FVector End = Character->GetActorLocation();

	TArray<FHitResult>HitResult;
	FCollisionQueryParams Params;

	// 자기 자신은 무시
	Params.AddIgnoredActor(GetOwner());

	// ECC_Camera 채널을 써서 카메라 콜리전에 반응하는 오브젝트만 감지
	GetWorld()->SweepMultiByChannel(HitResult,Start, End,FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(TraceRadius), Params);

	for (const FHitResult& Hit : HitResult)
	{
		AActor* HitActor = Hit.GetActor();
		if (!IsValid(HitActor)) continue;;

		// FadeableTag가 설정되어 있으면 해당 태그 있는 액터만 대상
		// None이면 모든 액터가 대상
		if (FadeableTag != NAME_None && !HitActor->ActorHasTag(FadeableTag))
		{
			continue;
		}

		Result.AddUnique(HitActor);
	}

	return Result;
}

void UCSCameraFadeComponent::CreateDynamicMaterials(FFadedActorInfo& Info)
{
	AActor* Actor = Info.Actor.Get();
	if (!IsValid(Actor)) return;

	// 액터가 가진 모든 스태틱 메시 컴포넌트(`UStaticMeshComponent`)를 수집
	TArray<UStaticMeshComponent*> MeshComponents;
	Actor->GetComponents<UStaticMeshComponent>(MeshComponents);

	// 수집된 컴포넌트들을 하나씩 순회하며 머티리얼 교체 작업 진행
	for (UStaticMeshComponent* Mesh : MeshComponents)
	{
		// 임시 저장용 배열 정의 (OrigSlots: 원본 복구용, DynamicSlots: 파라미터 조작용)
		TArray<TObjectPtr<UMaterialInterface>> OrigSlots;
		TArray<TObjectPtr<UMaterialInstanceDynamic>> DMISlots;

		// 메시가 가진 머티리얼 슬롯의 총 개수 파악
		const int32 NumMaterials = Mesh->GetNumMaterials();
		for (int32 i = 0; i < NumMaterials; i++)
		{
			// 추후 시야 차단 해제 시 원래대로 되돌리기 위해 원본 머티리얼 저장
			UMaterialInterface* Orig = Mesh->GetMaterial(i);
			OrigSlots.Add(Orig);

			//원본 머티리얼을 기반으로 런타임 수정이 가능한 다이나믹 머티리얼 인스턴스(DMI) 생성
			UMaterialInstanceDynamic* DMI = UMaterialInstanceDynamic::Create(Orig, Mesh);

			// 해당 슬롯의 머티리얼을 방금 만든 다이나믹 머티리얼로 교체 적용
			Mesh->SetMaterial(i, DMI);
			DMISlots.Add(DMI);
		}

		// MoveTemp를 활용해 임시 배열의 데이터를 구조체 내부 배열로 안전하고 빠르게 이동 (std::move)
		Info.OriginalMaterials.Add(MoveTemp(OrigSlots));
		Info.DynamicMaterials.Add(MoveTemp(DMISlots));
	}
}

void UCSCameraFadeComponent::RestoreOriginalMaterials(FFadedActorInfo& Info)
{
	AActor* Actor = Info.Actor.Get();
	if (!IsValid(Actor)) return;

	// 액터가 가진 모든 스태틱 메시 컴포넌트(`UStaticMeshComponent`)를 수집
	TArray<UStaticMeshComponent*> MeshComponents;
	Actor->GetComponents<UStaticMeshComponent>(MeshComponents);

	// (저장된 원본 머티리얼 데이터의 개수까지만 안전하게 순회)
	for (int32 i = 0; i < MeshComponents.Num() && i < Info.OriginalMaterials.Num(); i++)
	{
		// 현재 순서에 해당하는 스태틱 메시 컴포넌트
		UStaticMeshComponent* Mesh = MeshComponents[i];

		// 해당 메시가 원래 가지고 있던 머티리얼 슬롯 목록
		const TArray<TObjectPtr<UMaterialInterface>> OrigSlots = Info.OriginalMaterials[i];

		// 머티리얼 슬롯들을 순회하며 원래의 머티리얼을 하나씩 다시 적용
		for (int32 j = 0; j < OrigSlots.Num(); j++)
		{
			Mesh->SetMaterial(j, OrigSlots[j]);
		}
	}

}
