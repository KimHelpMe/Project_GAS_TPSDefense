// CSK

#pragma once

#include "CoreMinimal.h"
#include "Player/CSBasePlayerController.h"
#include "CSLobbyPlayerController.generated.h"

class ULobbyInteractionComponent;
class ULobbyWidget;

UCLASS()
class GASBASE_API ACSLobbyPlayerController : public ACSBasePlayerController
{
	GENERATED_BODY()

public:
	ACSLobbyPlayerController();

	FORCEINLINE ULobbyInteractionComponent* GetLobbyInteractionComponent() const { return LobbyInteractionComponent; }

	// 로비 위젯 인원 갱신
	void UpdateLobbyPlayerCount(int32 Count);

	// 로비를 떠나 타이틀 맵으로 복귀
	void RequestLeaveLobby();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// 베이스 오버라이드: 로비 위젯 안의 ChatWidget 반환
	virtual UChatWidget* GetChatWidget() const override;

	// LobbyInteractionComponent의 게임 시작 요청 수신
	UFUNCTION()
	void OnGameStartRequested();

	// GameState의 PlayerArray를 폴링하여 인원 변화 감지
	void PollPlayerCount();

	// Alt 키: 커서 토글 (Start 버튼 등 UI 클릭용)
	void ToggleCursor();

private:
	// 커서 토글용 입력 액션 (에디터에서 할당)
	UPROPERTY(EditDefaultsOnly, Category = "Input|UI")
	TObjectPtr<UInputAction> ToggleCursorAction;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULobbyWidget> LobbyWidgetClass;

	UPROPERTY()
	TObjectPtr<ULobbyWidget> LobbyWidget;

	UPROPERTY(VisibleAnywhere, Category = "Lobby")
	TObjectPtr<ULobbyInteractionComponent> LobbyInteractionComponent;

	FTimerHandle PlayerCountTimerHandle;

	int32 CachedPlayerCount = 0;
};
