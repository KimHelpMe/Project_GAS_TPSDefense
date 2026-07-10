// CSK

#include "Game/CSGameInstance.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

void UCSGameInstance::Init()
{
	Super::Init();

	if (GEngine)
	{
		NetworkFailureHandle = GEngine->OnNetworkFailure().AddUObject(
			this, &ThisClass::HandleNetworkFailure);
		TravelFailureHandle = GEngine->OnTravelFailure().AddUObject(
			this, &ThisClass::HandleTravelFailure);
	}
}

void UCSGameInstance::Shutdown()
{
	if (GEngine)
	{
		GEngine->OnNetworkFailure().Remove(NetworkFailureHandle);
		GEngine->OnTravelFailure().Remove(TravelFailureHandle);
	}

	Super::Shutdown();
}

void UCSGameInstance::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver,
	ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	UE_LOG(LogTemp, Warning, TEXT("Network Failure [%d]: %s"),
		static_cast<int32>(FailureType), *ErrorString);

	ReturnToMainMenu();
}

void UCSGameInstance::HandleTravelFailure(UWorld* World,
	ETravelFailure::Type FailureType, const FString& ErrorString)
{
	UE_LOG(LogTemp, Warning, TEXT("Travel Failure [%d]: %s"),
		static_cast<int32>(FailureType), *ErrorString);

	ReturnToMainMenu();
}

void UCSGameInstance::ReturnToMainMenu()
{
	// 세션 정리
	UMultiplayerSessionsSubsystem* SessionSub = GetSubsystem<UMultiplayerSessionsSubsystem>();
	if (SessionSub)
	{
		SessionSub->DestroySession();
	}

	// 타이틀 맵으로 이동
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("/MultiplayerSessions/Maps/Main"));
}
