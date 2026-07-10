// CSK

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CSGameInstance.generated.h"

UCLASS()
class GASBASE_API UCSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

private:
	// 네트워크 끊김 핸들러
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver,
		ENetworkFailure::Type FailureType, const FString& ErrorString);

	// 트래블 에러 핸들러
	void HandleTravelFailure(UWorld* World,
		ETravelFailure::Type FailureType, const FString& ErrorString);

	// 타이틀 맵으로 복귀
	void ReturnToMainMenu();

	FDelegateHandle NetworkFailureHandle;
	FDelegateHandle TravelFailureHandle;
};
