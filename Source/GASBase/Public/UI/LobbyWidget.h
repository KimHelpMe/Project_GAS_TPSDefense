// CSK

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

class UButton;
class UTextBlock;
class UChatWidget;
class UPlayerListWidget;

UCLASS()
class GASBASE_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 외부에서 호출: 인원 수 갱신
	void UpdatePlayerCount(int32 Count);

	// 외부에서 호출: 채팅 입력창 포커스
	void FocusChatInput();

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnStartButtonClicked();

private:
	// Start 버튼 (호스트만 Visible로 설정)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartButton;

	// 현재 인원 수 표시
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerCountText;

	// 조립된 하위 위젯 (블루프린트에서 배치)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UChatWidget> ChatWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPlayerListWidget> PlayerListWidget;
};
