// CSK


#include "Abilities/Player/CSPrimary.h"
#include "Engine/OverlapResult.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/CSGameplayTags.h"


TArray<AActor*>  UCSPrimary::HitBoxOverlap()
{
	/** C++ 전용 최적화 패턴
	* 블루프린트의 오버랩 노드는 기본적으로 모든 콜리전 채널에 대해 응답을 평가
	* 따라서 결과를 필터링하려면 캐스팅이나 인터페이스 체크 같은 추가 작업이 필요
	* 반면 C++에서는 FCollisionResponseParams를 직접 구성하여 쿼리 자체가 특정 오브젝트 타입만 감지하도록 제한 가능
	*/

	// 아바타 액터를 무시 목록에 추가
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());

	// 아바타 액터를 무시하도록 설정
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(ActorsToIgnore);

	// 응답 파라미터 구성 (Pawn 채널만 감지)
	FCollisionResponseParams ResponseParams;

	// 모든 채널을 먼저 Ignore로 초기화
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);

	// Pawn 오브젝트 타입에만 Block 응답 설정
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);

	// 오버랩 결과 배열 및 구체 준비
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);

	// 히트박스 위치 계산
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector() * ForwardOffset;
	const FVector HitBoxLocation = GetAvatarActorFromActorInfo()->GetActorLocation() + Forward + FVector(0.f, 0.f, ElevationOffset);

	// Visibility 채널로 구형 멀티 오버랩 수행
	GetWorld()->OverlapMultiByChannel(
		OverlapResults,       // 결과를 받을 배열
		HitBoxLocation,       // 구체 중심 위치
		FQuat::Identity,      // 회전 (구체이므로 무관)
		ECC_Visibility,       // 사용할 트레이스 채널
		Sphere,               // 구체 형상
		QueryParams,          // 무시할 액터 설정
		ResponseParams        // 채널별 응답 설정 (Pawn만 감지)
	);

	// 오버랩된 결과 중 유효한 액터만 골라 배열에 담는다
	TArray<AActor*> ActorsHit;
	for (const FOverlapResult& Result : OverlapResults)
	{
		if (!IsValid(Result.GetActor())) continue;
		ActorsHit.AddUnique(Result.GetActor()); // 같은 액터 중복 방지
	}

	// 디버그 시각화 (bDrawDebugs가 true일 때만)
	if (bDrawDebugs)
	{
		DrawHitBoxOverlapDebugs(OverlapResults, HitBoxLocation);
	}

	return ActorsHit;
}

void UCSPrimary::SendHitReactEventToActors(const TArray<AActor*>& ActorsHit)
{
	// ActorsHit은 이미 유효성 검사된 배열이므로 IsValid 재확인 불필요
	for (AActor* HitActor : ActorsHit)
	{
		FGameplayEventData Payload;
		Payload.Instigator = GetAvatarActorFromActorInfo(); // 공격을 유발한 주체(플레이어)를 페이로드에 담는다

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, CSGameplayTags::Events::HitReact, Payload);
	}
}

void UCSPrimary::DrawHitBoxOverlapDebugs(const TArray<FOverlapResult>& OverlapResults, const FVector& HitBoxLocation) const
{
	// 전체 히트박스 — 빨간 구체
	DrawDebugSphere(GetWorld(), HitBoxLocation, Radius, 16, FColor::Red, false, 3.f);

	// 감지된 액터별 — 초록 구체
	for (const FOverlapResult& Result : OverlapResults)
	{
		if (IsValid(Result.GetActor()))
		{
			FVector DebugLocation = Result.GetActor()->GetActorLocation();
			DebugLocation.Z += 100.f; // 캐릭터 중앙과 겹치지 않도록 위로 올림
			DrawDebugSphere(GetWorld(), DebugLocation, 30.f, 10, FColor::Green, false, 3.f);
		}
	}
}
