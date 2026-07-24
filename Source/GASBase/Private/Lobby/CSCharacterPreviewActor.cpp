// CSK

#include "Lobby/CSCharacterPreviewActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

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

void ACSCharacterPreviewActor::StartRunAnimation()
{
	UAnimInstance* AnimInst = PreviewMesh->GetAnimInstance();
	if (!AnimInst) return;

	// 재생 중인 몽타주 즉시 캔슬
	AnimInst->StopAllMontages(0.0f);

	// ABP의 Speed 변수 (double)에 직접 접근하여 달리기 전환
	FDoubleProperty* SpeedProp = FindFProperty<FDoubleProperty>(
		AnimInst->GetClass(), TEXT("Speed"));
	if (SpeedProp)
	{
		SpeedProp->SetPropertyValue_InContainer(AnimInst, 600.0);
	}
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

	// AnimBP 교체 → BlueprintBeginPlay 타이밍 보장을 위해 InitAnim 강제 호출
	if (Entry->PreviewAnimBP)
	{
		PreviewMesh->SetAnimInstanceClass(Entry->PreviewAnimBP);
		PreviewMesh->InitAnim(true);
	}

	// 실루엣 연출용 Custom Depth 활성화
	PreviewMesh->SetRenderCustomDepth(true);
	PreviewMesh->SetCustomDepthStencilValue(1);
	PreviewMesh->SetVisibility(true);
}
