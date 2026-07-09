// CSK

#include "Lobby/LobbyInteractionComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/World.h"

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
	//// 호스트(로컬 권한)만 요청 가능
	//APlayerController* PC = Cast<APlayerController>(GetOwner());
	//if (PC && PC->HasAuthority())
	//{
	//	ServerRequestStartGame();
	//}

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

	// GameMode에 시작 요청을 위임
	// LobbyInteractionComponent(GASBase 모듈)는 MultiplayerSessions 플러그인을 직접 참조하지 않고,
	// 세션 제어 권한이 있는 GameMode(플러그인 모듈)가 StartSession을 호출하도록 델리게이트로 연결
	OnGameStartRequested.Broadcast();

	// 전체 클라이언트에 시작 알림 (UI 전환 등에 활용)
	MulticastGameStarting();
}

void ULobbyInteractionComponent::MulticastGameStarting_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Game is starting... (UI Screen Blockor Active)"));
	//OnGameStartRequested.Broadcast();
}
