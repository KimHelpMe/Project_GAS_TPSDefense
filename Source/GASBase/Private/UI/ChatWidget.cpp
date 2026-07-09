// CSK

#include "UI/ChatWidget.h"
#include "Chat/ChatSubsystem.h"
#include "Chat/ChatComponent.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"

void UChatWidget::FocusChatInput()
{
	if (ChatInputBox)
	{
		ChatInputBox->SetKeyboardFocus();
	}
}

void UChatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 클릭 바인딩
	if (SendButton)
	{
		SendButton->OnClicked.AddDynamic(this, &ThisClass::OnSendClicked);
	}

	// 입력창 Enter 키 바인딩
	if (ChatInputBox)
	{
		ChatInputBox->OnTextCommitted.AddDynamic(this, &ThisClass::OnInputCommitted);
	}

	// Subsystem 캐싱 및 델리게이트 바인딩
	if (UGameInstance* GI = GetGameInstance())
	{
		ChatSubsystem = GI->GetSubsystem<UChatSubsystem>();
		if (ChatSubsystem)
		{
			ChatSubsystem->OnChatMessageReceived.AddDynamic(this, &ThisClass::OnChatMessageReceived);

			// 기존 채팅 기록 복원 (맵 전환 후 재접속 시)
			for (const FChatMessage& Msg : ChatSubsystem->GetHistory())
			{
				AddMessageToDisplay(Msg);
			}
		}
	}
}

void UChatWidget::NativeDestruct()
{
	// 델리게이트 해제
	if (ChatSubsystem)
	{
		ChatSubsystem->OnChatMessageReceived.RemoveDynamic(this, &ThisClass::OnChatMessageReceived);
	}

	Super::NativeDestruct();
}

void UChatWidget::OnChatMessageReceived(const FChatMessage& Message)
{
	AddMessageToDisplay(Message);
}

void UChatWidget::OnSendClicked()
{
	SendCurrentInput();
}

void UChatWidget::OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	// Enter 키로 전송
	if (CommitMethod == ETextCommit::OnEnter)
	{
		SendCurrentInput();
	}
}

void UChatWidget::SendCurrentInput()
{
	if (!ChatInputBox) return;

	FString InputText = ChatInputBox->GetText().ToString().TrimStartAndEnd();
	if (InputText.IsEmpty()) return;

	// PC에 부착된 ChatComponent를 찾아서 전송
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		if (UChatComponent* ChatComp = PC->FindComponentByClass<UChatComponent>())
		{
			ChatComp->SendChatMessage(InputText);
		}
	}

	// 입력창 초기화 및 포커스 복귀
	ChatInputBox->SetText(FText::GetEmpty());
	ChatInputBox->SetKeyboardFocus();
}

void UChatWidget::AddMessageToDisplay(const FChatMessage& Message)
{
	if (!MessageScrollBox) return;

	// 메시지 텍스트 블록 동적 생성
	UTextBlock* MessageText = NewObject<UTextBlock>(this);
	if (!MessageText) return;

	// "[발신자] 내용" 포맷
	FString DisplayText = FString::Printf(TEXT("[%s] %s"), *Message.SenderName, *Message.Content);
	MessageText->SetText(FText::FromString(DisplayText));

	// 스크롤박스에 추가 후 맨 아래로 스크롤
	MessageScrollBox->AddChild(MessageText);
	MessageScrollBox->ScrollToEnd();
}
