// CSK

#include "UI/PlayerListWidget.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

void UPlayerListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 위젯 생성 시 최초 1회 갱신
	RefreshPlayerList();
}

void UPlayerListWidget::RefreshPlayerList()
{
	if (!PlayerListBox) return;

	// 기존 목록 초기화
	PlayerListBox->ClearChildren();

	// GameState에서 현재 접속 중인 플레이어 목록 조회
	UWorld* World = GetWorld();
	if (!World) return;

	AGameStateBase* GS = World->GetGameState<AGameStateBase>();
	if (!GS) return;

	for (APlayerState* PS : GS->PlayerArray)
	{
		if (!PS) continue;

		UTextBlock* NameText = NewObject<UTextBlock>(this);
		if (!NameText) continue;

		NameText->SetText(FText::FromString(PS->GetPlayerName()));
		PlayerListBox->AddChild(NameText);
	}
}
