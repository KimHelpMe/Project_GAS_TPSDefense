// CSK


#include "Abilities/Enemy/CSHitReact.h"

void UCSHitReact::CacheHitDirectionVectors(AActor* Instigator)
{
	// 아바타(적)의 forward 벡터 캐싱
	AvatarForward = GetAvatarActorFromActorInfo()->GetActorForwardVector();

	const FVector AvatarLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector InstigatorLocation = Instigator->GetActorLocation();

	// 아바타에서 공격자를 향하는 벡터를 구하고 정규화
	ToInstigator = InstigatorLocation - AvatarLocation;
	ToInstigator.Normalize();
}
