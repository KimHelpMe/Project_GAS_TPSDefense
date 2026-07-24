// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSTitlePreviewActor.generated.h"

class USkeletalMeshComponent;

UCLASS()
class GASBASE_API ACSTitlePreviewActor : public AActor
{
	GENERATED_BODY()

public:
	ACSTitlePreviewActor();

protected:
	virtual void BeginPlay() override;

private:
	// 시작 몽타주 스톱 (한 프레임 딜레이 후 호출)
	void StopAllMontages();

	UPROPERTY(VisibleAnywhere, Category = "Preview")
	TObjectPtr<USkeletalMeshComponent> PreviewMesh;

	// 에디터에서 직접 메시/AnimBP 지정
	UPROPERTY(EditAnywhere, Category = "Preview")
	TObjectPtr<USkeletalMesh> Mesh;

	UPROPERTY(EditAnywhere, Category = "Preview")
	TSubclassOf<UAnimInstance> AnimBP;
};
