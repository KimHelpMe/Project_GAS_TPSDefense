// CSK

#pragma once

#include "CoreMinimal.h"
#include "Player/CSBasePlayerController.h"
#include "GameplayTagContainer.h"
#include "CSPlayerController.generated.h"

class UInputAction;
class UChatWidget;
class UCSRespawnWidget;
class UUserWidget;
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

	// GameOver 화면의 "재시작" 버튼에서 호출 — 서버가 현재 레벨을 다시 로드해서 모든 클라이언트를 트래블시킴
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "GAS|GameOver")
	void ServerRequestRestart();

	// GameOver 화면의 "로비로" 버튼에서 호출 — 서버가 로비 맵으로 트래블시킴
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "GAS|GameOver")
	void ServerRequestReturnToLobby();

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
