// CSK

#include "Lobby/LobbyInteractionComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/World.h"
#include "Player/CSPlayerState.h"

ULobbyInteractionComponent::ULobbyInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void ULobbyInteractionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// bIsReady를 모든 클라이언트에 리플리케이트
	DOREPLIFETIME(ULobbyInteractionComponent, bIsReady);
}

void ULobbyInteractionComponent::OnRep_IsReady()
{
	// 리플리케이션으로 값이 도착했을 때 UI 갱신 델리게이트 발동
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	APlayerState* PS = PC ? PC->PlayerState : nullptr;
	OnPlayerReadyChanged.Broadcast(PS, bIsReady);
}

void ULobbyInteractionComponent::ToggleReady()
{
	ServerToggleReady();
}

void ULobbyInteractionComponent::RequestStartGame()
{
	ServerRequestStartGame();
}

void ULobbyInteractionComponent::ServerToggleReady_Implementation()
{
	bIsReady = !bIsReady;

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	APlayerState* PS = PC ? PC->PlayerState : nullptr;

	// 전체 클라이언트에 동기화
	MulticastReadyChanged(PS, bIsReady);
}

void ULobbyInteractionComponent::MulticastReadyChanged_Implementation(APlayerState* Player, bool bReady)
{
	OnPlayerReadyChanged.Broadcast(Player, bReady);
}

void ULobbyInteractionComponent::ServerRequestStartGame_Implementation()
{
	// 서버 권한 확인
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC || !PC->HasAuthority()) return;

	// 모든 플레이어가 캐릭터를 선택했는지 확인
	AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>();
	if (!GameState) return;

	for (APlayerState* PS : GameState->PlayerArray)
	{
		ACSPlayerState* CSState = Cast<ACSPlayerState>(PS);
		if (!CSState || !CSState->SelectedCharacterClass)
		{
			// 한 명이라도 선택 안 했으면 시작 불가
			return;
		}
	}

	// GameMode에 시작 요청을 위임
	// LobbyInteractionComponent(GASBase 모듈)는 MultiplayerSessions 플러그인을 직접 참조하지 않고,
	// 세션 제어 권한이 있는 GameMode(플러그인 모듈)가 StartSession을 호출하도록 델리게이트로 연결
	OnGameStartRequested.Broadcast();

	// 전체 클라이언트에 시작 알림 (UI 전환 등에 활용)
	MulticastGameStarting();
}

void ULobbyInteractionComponent::MulticastGameStarting_Implementation()
{
}
