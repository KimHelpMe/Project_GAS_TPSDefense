// CSK

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerListWidget.generated.h"

class UVerticalBox;
class UTextBlock;

UCLASS()
class GASBASE_API UPlayerListWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 외부에서 호출: 플레이어 목록 갱신
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void RefreshPlayerList();

protected:
	virtual void NativeConstruct() override;

private:
	// 플레이어 이름 목록을 표시할 컨테이너
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> PlayerListBox;
};
