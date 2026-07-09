// CSK

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Chat/ChatTypes.h"
#include "ChatComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GASBASE_API UChatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UChatComponent();

	// 로컬 클라이언트가 호출하는 메시지 전송 함수
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SendChatMessage(const FString& Content, EChatChannel Channel = EChatChannel::All);

protected:
	// 클라이언트 → 서버: 메시지 전송 요청
	UFUNCTION(Server, Reliable)
	void ServerSendChatMessage(const FString& Content, EChatChannel Channel);

	// 서버 → 해당 클라이언트: 메시지 수신
	UFUNCTION(Client, Reliable)
	void ClientReceiveChatMessage(const FChatMessage& Message);

private:
	// 발신자 이름 획득 헬퍼
	FString GetSenderName() const;
};
