// CSK

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LobbyTransitionInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class ULobbyTransitionInterface : public UInterface
{
	GENERATED_BODY()
};

class MULTIPLAYERSESSIONS_API ILobbyTransitionInterface
{
	GENERATED_BODY()

public:
	// 배틀 트랜지션 시작 — 달리기 연출 (GameMode → PC → Client RPC)
	UFUNCTION(BlueprintNativeEvent, Category = "Battle Transition")
	void StartBattleTransition();

	// 배틀 트랜지션 종료 — 마름모 열림 (GameMode → PC → Client RPC)
	UFUNCTION(BlueprintNativeEvent, Category = "Battle Transition")
	void EndBattleTransition();
};
