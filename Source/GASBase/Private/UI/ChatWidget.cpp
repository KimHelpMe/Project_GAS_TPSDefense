// CSK

#include "UI/ChatWidget.h"
#include "Chat/ChatSubsystem.h"
#include "Chat/ChatComponent.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"


void UChatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SendButton)
	{
		SendButton->OnClicked.AddDynamic(this, &ThisClass::OnSendClicked);
	}

	if (ChatInputBox)
	{
		ChatInputBox->OnTextCommitted.AddDynamic(this, &ThisClass::OnInputCommitted);
	}

	// 서브시스템 바인딩 + 기존 히스토리 로드
	if (UGameInstance* GI = GetGameInstance())
	{
		ChatSubsystem = GI->GetSubsystem<UChatSubsystem>();
		if (ChatSubsystem)
		{
			ChatSubsystem->OnChatMessageReceived.AddDynamic(this, &ThisClass::OnChatMessageReceived);

			for (const FChatMessage& Msg : ChatSubsystem->GetHistory())
			{
				AddMessageToDisplay(Msg);
			}
		}
	}

	// 초기 상태: Idle (입력창 숨김, 메시지 영역 최소 투명도)
	SetInputAreaVisible(false);
	CurrentOpacity = 0.2f;
	if (MessageScrollBox)
	{
		MessageScrollBox->SetRenderOpacity(CurrentOpacity);
	}
	if (MessageBackground)
	{
		MessageBackground->SetRenderOpacity(CurrentOpacity);
	}
}

void UChatWidget::NativeDestruct()
{
	// 타이머 정리
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FadeTimerHandle);
		World->GetTimerManager().ClearTimer(FadeTickHandle);
	}

	if (ChatSubsystem)
	{
		ChatSubsystem->OnChatMessageReceived.RemoveDynamic(this, &ThisClass::OnChatMessageReceived);
	}

	Super::NativeDestruct();
}


void UChatWidget::ActivateChat()
{
	bChatActive = true;

	// 페이드 타이머 전부 중단
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FadeTimerHandle);
		World->GetTimerManager().ClearTimer(FadeTickHandle);
	}

	// 메시지 영역 완전 불투명
	ShowMessages();

	// 입력 영역 표시 + 포커스
	SetInputAreaVisible(true);
	if (ChatInputBox)
	{
		ChatInputBox->SetKeyboardFocus();
	}
}

void UChatWidget::DeactivateChat()
{
	bChatActive = false;

	// 입력창 초기화 및 숨김
	if (ChatInputBox)
	{
		ChatInputBox->SetText(FText::GetEmpty());
	}
	SetInputAreaVisible(false);

	// 페이드아웃 타이머 시작
	ResetFadeTimer();

	// PC에 비활성 요청
	OnChatDeactivateRequested.Broadcast();
}


void UChatWidget::OnSendClicked()
{
	SendCurrentInput();
	DeactivateChat();
}

void UChatWidget::OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		SendCurrentInput();
		DeactivateChat();
	}
	else if (CommitMethod == ETextCommit::OnCleared)
	{
		// Escape: 전송 없이 비활성
		DeactivateChat();
	}
}

void UChatWidget::SendCurrentInput()
{
	if (!ChatInputBox) return;

	FString InputText = ChatInputBox->GetText().ToString().TrimStartAndEnd();
	if (InputText.IsEmpty()) return;

	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		if (UChatComponent* ChatComp = PC->FindComponentByClass<UChatComponent>())
		{
			ChatComp->SendChatMessage(InputText);
		}
	}

	ChatInputBox->SetText(FText::GetEmpty());
}


void UChatWidget::OnChatMessageReceived(const FChatMessage& Message)
{
	AddMessageToDisplay(Message);

	// 비활성 상태에서도 새 메시지가 오면 잠깐 보여줌
	ShowMessages();
	ResetFadeTimer();
}

void UChatWidget::AddMessageToDisplay(const FChatMessage& Message)
{
	if (!MessageScrollBox) return;

	UTextBlock* MessageText = NewObject<UTextBlock>(this);
	if (!MessageText) return;

	FString DisplayText = FString::Printf(TEXT("[%s] %s"), *Message.SenderName, *Message.Content);
	MessageText->SetText(FText::FromString(DisplayText));

	// ChatInputBox에 설정된 폰트를 그대로 재사용
	if (ChatInputBox)
	{
		MessageText->SetFont(ChatInputBox->WidgetStyle.TextStyle.Font);
	}
	MessageText->SetAutoWrapText(true);

	MessageScrollBox->AddChild(MessageText);
	MessageScrollBox->ScrollToEnd();
}


void UChatWidget::ShowMessages()
{
	// 페이드 보간 타이머 중단
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FadeTickHandle);
	}

	CurrentOpacity = 1.f;

	if (MessageScrollBox)
	{
		MessageScrollBox->SetRenderOpacity(1.f);
	}
	if (MessageBackground)
	{
		MessageBackground->SetRenderOpacity(1.f);
	}
}

void UChatWidget::ResetFadeTimer()
{
	if (bChatActive) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FadeTimerHandle);
		World->GetTimerManager().ClearTimer(FadeTickHandle);
		World->GetTimerManager().SetTimer(
			FadeTimerHandle,
			this,
			&ThisClass::StartFadeOut,
			FadeDelay,
			false
		);
	}
}

void UChatWidget::StartFadeOut()
{
	if (bChatActive) return;

	static constexpr float TickInterval = 1.f / 60.f;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			FadeTickHandle,
			this,
			&ThisClass::FadeOutTick,
			TickInterval,
			true
		);
	}
}

void UChatWidget::FadeOutTick()
{
	static constexpr float TickInterval = 1.f / 60.f;

	CurrentOpacity -= TickInterval / FadeDuration;

	if (CurrentOpacity <= 0.2f)
	{
		CurrentOpacity = 0.2f;

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(FadeTickHandle);
		}
	}

	if (MessageScrollBox)
	{
		MessageScrollBox->SetRenderOpacity(CurrentOpacity);
	}
	if (MessageBackground)
	{
		MessageBackground->SetRenderOpacity(CurrentOpacity);
	}
}

void UChatWidget::SetInputAreaVisible(bool bVisible)
{
	ESlateVisibility NewVisibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;

	if (ChatInputBox) ChatInputBox->SetVisibility(NewVisibility);
	if (SendButton) SendButton->SetVisibility(NewVisibility);
}
