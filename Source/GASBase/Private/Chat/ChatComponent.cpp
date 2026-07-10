// CSK

#include "Chat/ChatComponent.h"
#include "Chat/ChatSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/World.h"

UChatComponent::UChatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// 네트워크 리플리케이션 활성화
	SetIsReplicatedByDefault(true);
}

void UChatComponent::SendChatMessage(const FString& Content, EChatChannel Channel)
{
	if (Content.IsEmpty()) return;

	// 서버에 전송 요청
	ServerSendChatMessage(Content, Channel);
}

void UChatComponent::ServerSendChatMessage_Implementation(const FString& Content, EChatChannel Channel)
{
	// 서버에서 메시지 객체 조립
	FChatMessage NewMessage;
	NewMessage.SenderName = GetSenderName();
	NewMessage.Content = Content;
	NewMessage.Channel = Channel;
	NewMessage.Timestamp = GetWorld()->GetTimeSeconds();

	// 모든 PC의 ChatComponent에 브로드캐스트
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC) continue;

		UChatComponent* OtherChat = PC->FindComponentByClass<UChatComponent>();
		if (OtherChat)
		{
			OtherChat->ClientReceiveChatMessage(NewMessage);
		}
	}
}

void UChatComponent::ClientReceiveChatMessage_Implementation(const FChatMessage& Message)
{
	// 로컬 Subsystem에 기록 추가 → 델리게이트 → UI 자동 갱신
	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (UChatSubsystem* ChatSub = GI->GetSubsystem<UChatSubsystem>())
		{
			ChatSub->AddMessage(Message);
		}
	}
}

FString UChatComponent::GetSenderName() const
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (PC && PC->PlayerState)
	{
		return PC->PlayerState->GetPlayerName();
	}
	return TEXT("Unknown");
}
