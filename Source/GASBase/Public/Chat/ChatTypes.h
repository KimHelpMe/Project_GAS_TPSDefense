// CSK

#pragma once

#include "CoreMinimal.h"
#include "ChatTypes.generated.h"

// 채팅 채널 구분 (확장 대비)
UENUM(BlueprintType)
enum class EChatChannel : uint8
{
	All		UMETA(DisplayName = "All"),
	Team	UMETA(DisplayName = "Team"),
	System	UMETA(DisplayName = "System")
};

// 채팅 메시지 단위 구조체
USTRUCT(BlueprintType)
struct FChatMessage
{
	GENERATED_BODY()

	// 발신자 이름
	UPROPERTY(BlueprintReadOnly)
	FString SenderName = TEXT("");

	// 메시지 본문
	UPROPERTY(BlueprintReadOnly)
	FString Content = TEXT("");

	// 채널
	UPROPERTY(BlueprintReadOnly)
	EChatChannel Channel = EChatChannel::All;

	// 타임스탬프 (서버 기준)
	UPROPERTY(BlueprintReadOnly)
	float Timestamp = 0.f;
};
