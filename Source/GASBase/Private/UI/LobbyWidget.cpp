// CSK

#include "UI/LobbyWidget.h"
#include "UI/ChatWidget.h"
#include "UI/PlayerListWidget.h"
#include "Lobby/LobbyInteractionComponent.h"
#include "Player/CSLobbyPlayerController.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Start 버튼 클릭 바인딩
	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &ThisClass::OnStartButtonClicked);

		// 호스트만 Start 버튼 표시
		APlayerController* PC = GetOwningPlayer();
		if (PC && PC->HasAuthority())
		{
			StartButton->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			StartButton->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Leave 버튼 클릭 바인딩
	if (LeaveButton)
	{
		LeaveButton->OnClicked.AddDynamic(this, &ThisClass::OnLeaveButtonClicked);
	}
}

void ULobbyWidget::UpdatePlayerCount(int32 Count)
{
	if (PlayerCountText)
	{
		PlayerCountText->SetText(FText::FromString(FString::Printf(TEXT("Players: %d"), Count)));
	}

	// 플레이어 리스트 위젯도 함께 갱신
	if (PlayerListWidget)
	{
		PlayerListWidget->RefreshPlayerList();
	}
}

void ULobbyWidget::OnLeaveButtonClicked()
{
	ACSLobbyPlayerController* LobbyPC = Cast<ACSLobbyPlayerController>(GetOwningPlayer());
	if (LobbyPC)
	{
		LobbyPC->RequestLeaveLobby();
	}
}

void ULobbyWidget::OnStartButtonClicked()
{
	if (StartButton)
	{
		StartButton->SetIsEnabled(false);
	}

	// LobbyInteractionComponent를 통해 서버에 시작 요청
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		if (ULobbyInteractionComponent* LobbyComp = PC->FindComponentByClass<ULobbyInteractionComponent>())
		{
			LobbyComp->RequestStartGame();
		}
	}
}
