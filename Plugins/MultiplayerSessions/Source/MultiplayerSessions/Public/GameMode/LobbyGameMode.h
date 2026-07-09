

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ALobbyGameMode();


	// 유저가 세션 접속에 성공하여 완전히 로그인된 직후 호출 (컨트롤러 접근 안전지대)
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 유저가 게임을 나갈 때 호출
	virtual void Logout(AController* Exiting) override;
};
