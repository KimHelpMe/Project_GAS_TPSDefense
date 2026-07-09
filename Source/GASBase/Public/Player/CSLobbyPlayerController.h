// CSK

#pragma once

#include "CoreMinimal.h"
#include "Player/CSBasePlayerController.h"
#include "CSLobbyPlayerController.generated.h"

class UInputAction;
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

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// LobbyInteractionComponent의 게임 시작 요청 수신 → Subsystem 연결
	UFUNCTION()
	void OnGameStartRequested();

	// GameState의 PlayerArray를 폴링하여 인원 변화 감지
	void PollPlayerCount();

	// UI 커서 토글 (Alt / Enter)
	void ToggleCursor();

private:
	// UI 토글용 입력 액션 (에디터에서 할당)
	UPROPERTY(EditDefaultsOnly, Category = "Input|UI")
	TObjectPtr<UInputAction> ToggleCursorAction;

	// 로비 위젯 클래스 (에디터에서 블루프린트 위젯 할당)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULobbyWidget> LobbyWidgetClass;

	// 생성된 로비 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<ULobbyWidget> LobbyWidget;

	// 로비 전용 상호작용 컴포넌트 (Ready, Start)
	UPROPERTY(VisibleAnywhere, Category = "Lobby")
	TObjectPtr<ULobbyInteractionComponent> LobbyInteractionComponent;

	// 인원 폴링 타이머
	FTimerHandle PlayerCountTimerHandle;

	// 이전 인원 수 캐싱 (변경 시에만 위젯 갱신)
	int32 CachedPlayerCount = 0;

	// 현재 커서 활성 상태
	bool bCursorActive = false;
};
