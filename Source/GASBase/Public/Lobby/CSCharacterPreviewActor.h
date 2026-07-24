// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/CSCharacterSelectData.h"
#include "CSCharacterPreviewActor.generated.h"

class USkeletalMeshComponent;
class UCSCharacterSelectData;

UCLASS()
class GASBASE_API ACSCharacterPreviewActor : public AActor
{
	GENERATED_BODY()

public:
	ACSCharacterPreviewActor();

	// 캐릭터 전환 (None 이면 메시 숨김)
	UFUNCTION(BlueprintCallable, Category = "Lobby|Preview")
	void SetCharacter(ECSCharacterID InID);

	// 배틀 트랜지션 연출용 달리기 애니메이션 전환
	UFUNCTION(BlueprintCallable, Category = "Lobby|Preview")
	void StartRunAnimation();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Preview")
	TObjectPtr<USkeletalMeshComponent> PreviewMesh;

	// 에디터에서 DA_CharacterSelectData 할당
	UPROPERTY(EditAnywhere, Category = "Preview")
	TObjectPtr<UCSCharacterSelectData> CharacterSelectData;

	ECSCharacterID CurrentID = ECSCharacterID::None;
};
