// CSK

#include "Lobby/CSTitlePreviewActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

ACSTitlePreviewActor::ACSTitlePreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PreviewMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PreviewMesh"));
	SetRootComponent(PreviewMesh);
}

void ACSTitlePreviewActor::BeginPlay()
{
	Super::BeginPlay();

	if (Mesh)
	{
		PreviewMesh->SetSkeletalMesh(Mesh);
	}

	if (AnimBP)
	{
		PreviewMesh->SetAnimInstanceClass(AnimBP);
	}

	// Custom Depth 활성화 (포스트 프로세스 마스크용)
	PreviewMesh->SetRenderCustomDepth(true);
	PreviewMesh->SetCustomDepthStencilValue(1);

	// AnimBP 초기화 직후 시작 몽타주가 재생될 수 있어서 한 프레임 뒤에 스톱
	GetWorldTimerManager().SetTimerForNextTick(this, &ACSTitlePreviewActor::StopAllMontages);
}

void ACSTitlePreviewActor::StopAllMontages()
{
	if (UAnimInstance* AnimInst = PreviewMesh->GetAnimInstance())
	{
		AnimInst->StopAllMontages(0.f);
	}
}
