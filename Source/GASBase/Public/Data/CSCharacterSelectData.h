// CSK

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CSCharacterSelectData.generated.h"

// 선택 가능한 캐릭터 ID
UENUM(BlueprintType)
enum class ECSCharacterID : uint8
{
	None		UMETA(DisplayName = "None"),
	Twinblast	UMETA(DisplayName = "Twinblast"),
	Revenant	UMETA(DisplayName = "Revenant"),
};

// 캐릭터 한 종류의 프리뷰 데이터
USTRUCT(BlueprintType)
struct FCSCharacterSelectEntry
{
	GENERATED_BODY()

	// 프리뷰에 표시할 스켈레탈 메시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USkeletalMesh> PreviewMesh;

	// 대기 애니메이션용 AnimBP
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UAnimInstance> PreviewAnimBP;

	// 실제 게임에서 스폰할 캐릭터 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<APawn> CharacterClass;

	// 선택 버튼에 표시할 아이콘 텍스처
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> IconTexture;

	// UI에 표시할 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;
};

// DA_CharacterSelectData 에 이 DataAsset을 할당
UCLASS()
class GASBASE_API UCSCharacterSelectData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// ID → 엔트리 조회 (C++ 전용 - UHT는 USTRUCT 포인터 반환을 BlueprintCallable에서 허용하지 않음)
	const FCSCharacterSelectEntry* FindEntryByID(ECSCharacterID InID) const;

	// CharacterClass → ID 역방향 조회 (WBP_LobbyWidget에서 OnCharacterSelected 수신 후 PreviewActor에 전달할 때 사용)
	UFUNCTION(BlueprintCallable, Category = "CharacterSelect")
	ECSCharacterID FindIDByClass(TSubclassOf<APawn> InClass) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterSelect")
	TMap<ECSCharacterID, FCSCharacterSelectEntry> Entries;
};
