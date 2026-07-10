// CSK

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "Components/ActorComponent.h"
#include "LobbyInteractionComponent.generated.h"

// 로비 상태 변경 시 UI에 알릴 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerReadyChanged, APlayerState*, Player, bool, bIsReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStartRequested);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GASBASE_API ULobbyInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULobbyInteractionComponent();

	// 클라이언트가 호출: Ready 토글
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void ToggleReady();

	// 호스트만 호출: 게임 시작 요청
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void RequestStartGame();

	// 리플리케이션 등록
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Ready 상태 조회
	UFUNCTION(BlueprintPure, Category = "Lobby")
	bool IsReady() const { return bIsReady; }

	// UI 바인딩용 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnPlayerReadyChanged OnPlayerReadyChanged;

	UPROPERTY(BlueprintAssignable)
	FOnGameStartRequested OnGameStartRequested;

protected:
	// Ready 토글 RPC
	UFUNCTION(Server, Reliable)
	void ServerToggleReady();

	// 서버 → 전체 클라이언트: Ready 상태 동기화
	UFUNCTION(NetMulticast, Reliable)
	void MulticastReadyChanged(APlayerState* Player, bool bReady);

	// 게임 시작 RPC (호스트만 서버에 요청)
	UFUNCTION(Server, Reliable)
	void ServerRequestStartGame();

	// 서버 → 전체 클라이언트: 게임 시작 알림
	UFUNCTION(NetMulticast, Reliable)
	void MulticastGameStarting();

private:
	// Ready 상태 (리플리케이트 → 중간 입장 클라이언트도 즉시 동기화)
	UPROPERTY(ReplicatedUsing = OnRep_IsReady)
	bool bIsReady = false;

	// bIsReady 값이 서버로부터 변경되어 도착했을 때 호출
	UFUNCTION()
	void OnRep_IsReady();
};
