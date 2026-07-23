// CSK

#pragma once

#include "CoreMinimal.h"
#include "Player/CSBasePlayerController.h"
#include "GameplayTagContainer.h"
#include "Game/CSMatchResult.h"
#include "CSPlayerController.generated.h"

class UInputAction;
class UChatWidget;
class UCSRespawnWidget;
class UUserWidget;
class UCSAttributeWidget;
class ACSEnemyCharacter;
struct FInputActionValue;

UCLASS()
class GASBASE_API ACSPlayerController : public ACSBasePlayerController
{
	GENERATED_BODY()

public:
	// 서버 → 클라이언트: 마름모 열림 연출
	UFUNCTION(Client, Reliable)
	void Client_EndBattleTransition();

	// BP에서 마름모 열림 연출 구현
	UFUNCTION(BlueprintImplementableEvent, Category = "Battle Transition")
	void BP_EndBattleTransition();

	// 본진(Base) 파괴 시 BP_GameMode가 모든 클라이언트의 PlayerController에 호출 — 패배 화면 표시 + 입력 차단
	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "GAS|GameOver")
	void ClientShowGameOver();

	// 보스 사망 시 서버 → 클라이언트: 승리 화면 표시
	UFUNCTION(Client, Reliable)
	void Client_NotifyVictory(float LobbyDelay, FCSMatchResult MatchResult);

	// BP에서 승리 위젯 생성 + 카운트다운 구현
	UFUNCTION(BlueprintImplementableEvent, Category = "Victory")
	void BP_ShowVictoryScreen(float LobbyDelay, const FCSMatchResult& MatchResult);

	// GameOver 화면의 "재시작" 버튼에서 호출 — 서버가 현재 레벨을 다시 로드해서 모든 클라이언트를 트래블시킴
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "GAS|GameOver")
	void ServerRequestRestart();

	// GameOver 화면의 "로비로" 버튼에서 호출 — 서버가 로비 맵으로 트래블시킴
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "GAS|GameOver")
	void ServerRequestReturnToLobby();

	// 보스 출현 시 화면 고정 보스 체력바 UI를 표시 (없으면 생성), Boss의 Health/MaxHealth에 바인딩
	void ShowBossHPBar(ACSEnemyCharacter* Boss);

	// 보스 사망 시 보스 체력바 UI를 숨김
	void HideBossHPBar();

	// Input 설정 초기화
	void InitInput();

	// 서버 → 클라이언트: SeamlessTravel 후 입력 복구
	UFUNCTION(Client, Reliable)
	void Client_RestoreInput();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// 베이스 오버라이드: 인게임 ChatWidget 반환
	virtual UChatWidget* GetChatWidget() const override;

private:
	void ActivateAbility(const FGameplayTag& AbilityTag) const;

	UFUNCTION(Server, Reliable)
	void ServerSendGameplayEvent(FGameplayTag EventTag);

	void Roll();
	void LMB();
	void RMB();

	void Q();
	void E();
	void R();
	void T();

	void TestAbility();
	void StartSprint();
	void StopSprint();

private:
	// 인게임 채팅 위젯 클래스 (에디터에서 할당)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UChatWidget> ChatWidgetClass;

	// 생성된 인게임 채팅 위젯
	UPROPERTY()
	TObjectPtr<UChatWidget> InGameChatWidget;


	// 리스폰 카운트다운 위젯 클래스 (에디터에서 할당)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCSRespawnWidget> RespawnWidgetClass;

	// 생성된 리스폰 카운트다운 위젯
	UPROPERTY()
	TObjectPtr<UCSRespawnWidget> RespawnWidget;

	// 본진 파괴 시 표시할 패배 화면 위젯 클래스 (에디터에서 할당)
	UPROPERTY(EditDefaultsOnly, Category = "UI|GameOver")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	// 화면 고정 보스 체력바 위젯 클래스 (에디터에서 WBP_BossHPBar 할당)
	UPROPERTY(EditDefaultsOnly, Category = "UI|Boss")
	TSubclassOf<UCSAttributeWidget> BossHPBarWidgetClass;

	// 생성된 보스 체력바 위젯
	UPROPERTY()
	TObjectPtr<UCSAttributeWidget> BossHPBarWidget;


	UPROPERTY(EditDefaultsOnly, Category = "Input|Movement")
	TObjectPtr<UInputAction> RollAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Movement")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> PrimaryAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> SecondaryAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> QAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> EAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> RAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> TAction;


};
