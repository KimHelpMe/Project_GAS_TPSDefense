// CSK

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Chat/ChatTypes.h"
#include "ChatWidget.generated.h"

class UScrollBox;
class UEditableTextBox;
class UButton;
class UTextBlock;
class UChatSubsystem;

UCLASS()
class GASBASE_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 외부에서 호출: 입력창에 포커스 주기
	void FocusChatInput();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// 메시지 수신 콜백
	UFUNCTION()
	void OnChatMessageReceived(const FChatMessage& Message);

	// 전송 버튼 클릭
	UFUNCTION()
	void OnSendClicked();

	// 입력창에서 Enter 키 처리
	UFUNCTION()
	void OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod);

private:
	// 입력된 텍스트를 전송하고 입력창 초기화
	void SendCurrentInput();

	// 메시지를 스크롤박스에 추가하는 헬퍼
	void AddMessageToDisplay(const FChatMessage& Message);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> MessageScrollBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ChatInputBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SendButton;

	// Subsystem 캐싱
	UPROPERTY()
	TObjectPtr<UChatSubsystem> ChatSubsystem;
};
