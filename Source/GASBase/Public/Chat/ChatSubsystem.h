// CSK

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Chat/ChatTypes.h"
#include "ChatSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChatMessageReceived, const FChatMessage&, Message);

UCLASS()
class GASBASE_API UChatSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// 메시지 추가 (ChatComponent가 호출)
	void AddMessage(const FChatMessage& Message);

	// 기록 조회
	const TArray<FChatMessage>& GetHistory() const { return ChatHistory; }

	// 기록 초기화 (맵 전환 시 필요하면 호출)
	void ClearHistory();

	// UI가 바인딩할 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnChatMessageReceived OnChatMessageReceived;

private:
	// 채팅 기록 배열
	TArray<FChatMessage> ChatHistory;

	// 기록 최대 보관 수 (메모리 보호)
	static constexpr int32 MaxHistoryCount = 200;
};
