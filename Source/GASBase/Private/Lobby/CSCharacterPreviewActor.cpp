// CSK

#include "Lobby/CSCharacterPreviewActor.h"
#include "Components/SkeletalMeshComponent.h"

ACSCharacterPreviewActor::ACSCharacterPreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PreviewMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PreviewMesh"));
	SetRootComponent(PreviewMesh);

	// 처음엔 아무것도 표시하지 않음
	PreviewMesh->SetVisibility(false);
}

void ACSCharacterPreviewActor::BeginPlay()
{
	Super::BeginPlay();
}

void ACSCharacterPreviewActor::SetCharacter(ECSCharacterID InID)
{
	if (!CharacterSelectData) return;
	if (CurrentID == InID) return;

	CurrentID = InID;

	if (InID == ECSCharacterID::None)
	{
		PreviewMesh->SetSkeletalMesh(nullptr);
		PreviewMesh->SetVisibility(false);
		PreviewMesh->SetRenderCustomDepth(false);
		return;
	}

	const FCSCharacterSelectEntry* Entry = CharacterSelectData->FindEntryByID(InID);
	if (!Entry) return;

	// 메시 교체
	PreviewMesh->SetSkeletalMesh(Entry->PreviewMesh);

	// AnimBP 교체 → 대기 애니메이션 자동 재생
	if (Entry->PreviewAnimBP)
	{
		PreviewMesh->SetAnimInstanceClass(Entry->PreviewAnimBP);
	}

	// 실루엣 연출용 Custom Depth 활성화
	PreviewMesh->SetRenderCustomDepth(true);
	PreviewMesh->SetCustomDepthStencilValue(1);
	PreviewMesh->SetVisibility(true);
}
