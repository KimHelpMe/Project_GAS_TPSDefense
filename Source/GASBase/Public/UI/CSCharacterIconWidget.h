// CSK

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CSCharacterIconWidget.generated.h"

class UCSCharacterSelectData;
class APawn;

// 캐릭터 아이콘을 표시하는 위젯들의 C++ 베이스.
// WBP_PlayerStatus, WBP_TeamMemberEntry가 이 클래스를 부모로 하여 CharacterSelectData를 공유한다.
UCLASS()
class GASBASE_API UCSCharacterIconWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// SelectedCharacterClass에 해당하는 아이콘 텍스처 조회. CharacterSelectData가 비어있으면 nullptr.
	UFUNCTION(BlueprintPure, Category = "CharacterSelect")
	UTexture2D* GetIconTexture(TSubclassOf<APawn> InCharacterClass) const;

protected:
	// DA_CharacterSelectData 할당 (에디터에서 이 클래스를 부모로 하는 위젯 블루프린트의 Class Defaults에서 설정)
	UPROPERTY(EditDefaultsOnly, Category = "CharacterSelect")
	TObjectPtr<UCSCharacterSelectData> CharacterSelectData;
};
