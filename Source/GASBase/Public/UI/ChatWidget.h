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
class UBorder;
class UChatSubsystem;

// 채팅 비활성 요청 (전송 완료 또는 Escape 시 PC에 알림)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChatDeactivateRequested);

UCLASS()
class GASBASE_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// --- 외부(PC)에서 호출하는 상태 제어 API ---

	// Enter 입력 시: 활성 상태 진입 (입력창 표시 + 포커스 + 페이드 중단)
	void ActivateChat();

	// 전송/Escape 후: 비활성 상태 복귀 (입력창 숨김 + 페이드 타이머 시작)
	void DeactivateChat();

	// PC가 바인딩할 델리게이트
	UPROPERTY()
	FOnChatDeactivateRequested OnChatDeactivateRequested;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// 메시지 수신 콜백
	UFUNCTION()
	void OnChatMessageReceived(const FChatMessage& Message);

	// 전송 버튼 클릭
	UFUNCTION()
	void OnSendClicked();

	// 입력창에서 Enter / Escape 처리
	UFUNCTION()
	void OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod);

private:
	// 입력된 텍스트를 전송하고 입력창 초기화
	void SendCurrentInput();

	// 메시지를 스크롤박스에 추가하는 헬퍼
	void AddMessageToDisplay(const FChatMessage& Message);

	// --- 페이드 제어 ---

	// 메시지 영역을 즉시 불투명하게 표시
	void ShowMessages();

	// 페이드아웃 타이머를 (재)시작
	void ResetFadeTimer();

	// 페이드아웃 시작 (대기 타이머 만료 시 호출)
	void StartFadeOut();

	// 페이드아웃 보간 틱 (소간격 타이머)
	void FadeOutTick();

	// 입력 영역(입력창 + 전송 버튼) 표시/숨김
	void SetInputAreaVisible(bool bVisible);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> MessageScrollBox;

	// 메시지 영역 배경 (ScrollBox를 감싸는 Border)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> MessageBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ChatInputBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SendButton;

	UPROPERTY()
	TObjectPtr<UChatSubsystem> ChatSubsystem;

	// --- 상태 ---

	// 채팅 활성 상태 (입력 중)
	bool bChatActive = false;

	// --- 페이드 ---

	// 페이드아웃 대기 타이머
	FTimerHandle FadeTimerHandle;

	// 페이드아웃 보간 타이머
	FTimerHandle FadeTickHandle;

	// 현재 메시지 영역 목표 투명도
	float CurrentOpacity = 0.f;

	// 메시지 표시 후 페이드아웃 시작까지 대기 시간 (초)
	UPROPERTY(EditDefaultsOnly, Category = "Chat|Fade")
	float FadeDelay = 7.f;

	// 페이드아웃 소요 시간 (초)
	UPROPERTY(EditDefaultsOnly, Category = "Chat|Fade")
	float FadeDuration = 1.5f;
};
