// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

class UMultiplayerSessionsSubsystem;

UCLASS()
class MULTIPLAYERSESSIONS_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ALobbyGameMode();

	// 유저가 세션 접속에 성공하여 완전히 로그인된 직후 호출
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 유저가 게임을 나갈 때 호출
	virtual void Logout(AController* Exiting) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// StartSession 완료 콜백
	UFUNCTION()
	void OnStartSessionComplete(bool bWasSuccessful);

private:
	// 본 게임 맵 경로 (에디터에서 설정 가능)
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	FString GameMapPath = TEXT("/Game/TaehosWork/Maps/LV_PlayLevel");
};
