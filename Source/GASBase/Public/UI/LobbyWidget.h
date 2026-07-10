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

	// ChatWidget 접근자
	UChatWidget* GetChatWidget() const { return ChatWidget; }

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnStartButtonClicked();

	UFUNCTION()
	void OnLeaveButtonClicked();

private:
	// Start 버튼 (호스트만 Visible로 설정)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartButton;

	// 타이틀로 돌아가기 버튼
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LeaveButton;

	// 현재 인원 수 표시
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerCountText;

	// 조립된 하위 위젯 (블루프린트에서 배치)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UChatWidget> ChatWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPlayerListWidget> PlayerListWidget;
};
