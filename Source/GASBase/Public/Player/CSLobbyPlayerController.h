// CSK

#pragma once

#include "CoreMinimal.h"
#include "Player/CSBasePlayerController.h"
#include "LobbyTransitionInterface.h"
#include "CSLobbyPlayerController.generated.h"

class ULobbyInteractionComponent;
class ULobbyWidget;

UCLASS()
class GASBASE_API ACSLobbyPlayerController : public ACSBasePlayerController, public ILobbyTransitionInterface
{
	GENERATED_BODY()

public:
	ACSLobbyPlayerController();

	FORCEINLINE ULobbyInteractionComponent* GetLobbyInteractionComponent() const { return LobbyInteractionComponent; }

	// 로비 위젯 인원 갱신
	void UpdateLobbyPlayerCount(int32 Count);

	// 로비를 떠나 타이틀 맵으로 복귀
	void RequestLeaveLobby();

	// ILobbyTransitionInterface 구현
	// GameMode가 Execute_StartBattleTransition 호출 시 진입점
	virtual void StartBattleTransition_Implementation() override;

	// 서버 → 클라이언트: 달리기 연출 시작
	UFUNCTION(Client, Reliable)
	void Client_StartBattleTransition();

	// BP에서 달리기 연출 구현
	UFUNCTION(BlueprintImplementableEvent, Category = "Battle Transition")
	void BP_StartBattleTransition();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnChatDeactivated() override;

	// 베이스 오버라이드: 로비 위젯 안의 ChatWidget 반환
	virtual UChatWidget* GetChatWidget() const override;

	// LobbyInteractionComponent의 게임 시작 요청 수신
	UFUNCTION()
	void OnGameStartRequested();

	// GameState의 PlayerArray를 폴링하여 인원 변화 감지
	void PollPlayerCount();

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULobbyWidget> LobbyWidgetClass;

	UPROPERTY()
	TObjectPtr<ULobbyWidget> LobbyWidget;

	UPROPERTY(VisibleAnywhere, Category = "Lobby")
	TObjectPtr<ULobbyInteractionComponent> LobbyInteractionComponent;

	FTimerHandle PlayerCountTimerHandle;

	int32 CachedPlayerCount = 0;
};
