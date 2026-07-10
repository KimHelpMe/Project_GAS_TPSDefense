// CSK

#include "Chat/ChatSubsystem.h"

void UChatSubsystem::AddMessage(const FChatMessage& Message)
{
	// 최대치 초과 시 가장 오래된 메시지 제거
	if (ChatHistory.Num() >= MaxHistoryCount)
	{
		ChatHistory.RemoveAt(0);
	}

	ChatHistory.Add(Message);
	OnChatMessageReceived.Broadcast(Message);
}

void UChatSubsystem::ClearHistory()
{
	ChatHistory.Empty();
}
