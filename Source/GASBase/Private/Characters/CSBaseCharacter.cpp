// CSK

#include "Characters/CSBaseCharacter.h"


ACSBaseCharacter::ACSBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// 렌더링 여부와 관계없이 본 트랜스폼을 항상 갱신 (전용 서버에서 본 업데이트를 위해)
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

