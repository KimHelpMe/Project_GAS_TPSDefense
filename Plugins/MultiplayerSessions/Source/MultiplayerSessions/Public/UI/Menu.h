// CSK

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "Menu.generated.h"

class UButton;
class UMultiplayerSessionsSubsystem;

UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	// 이 위젯을 생성한 후 호출할 초기화 함수
	// UE 5.7 스타일: 매개변수 기본값을 선언부에 기술하여 블루프린트 노드를 깔끔하게 유지
	UFUNCTION(BlueprintCallable, Category = "Multiplayer Sessions")
	void MenuSetup(
		int32 NumberOfPublicConnections = 4,
		FString TypeOfMatch = TEXT("FreeForAll"),
		FString LobbyPath = TEXT("/MultiplayerSessions/Maps/Lobby")
	);

protected:
	// 위젯이 생성 및 초기화될 때 호출되는 엔진 라이프사이클 함수 오버라이드
	virtual bool Initialize() override;

	// OnLevelRemovedFromWorld 대체 함수
	virtual void NativeDestruct() override;

protected:

	// 서브시스템의 세션 결과를 가로챌 수신용 내부 콜백 함수 정의

	UFUNCTION() 
	void OnCreateSession(bool bWasSuccessful);

	// 일반 델리게이트 연동 (UFUNCTION 불필요)
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION() 
	void OnDestroySession(bool bWasSuccessful);

	UFUNCTION() 
	void OnStartSession(bool bWasSuccessful);

	UFUNCTION()
	void OnHostButtonClicked();

	UFUNCTION()
	void OnJoinButtonClicked();

private:
	// 메뉴를 해제하고 입력 모드를 원래대로 복구
	void MenuTearDown();

private:
	// meta = (BindWidget) 스펙은 블루프린트 에셋의 컴포넌트 이름과 C++ 변수명이 일치해야 합니다.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> HostButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinButton;

	// 세션 관리 플러그인 서브시스템 포인터 캐싱 변수
	UPROPERTY()
	TObjectPtr<UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;

	// 유저가 설정한 세션 변수를 기억해 둘 내부 캐싱 변수
	int32 NumPublicConnections{ 4 };
	FString MatchType{ TEXT("FreeForAll") };

	// 포맷팅 완료된 최종 이동 경로 (?listen 옵션이 포함된 문자열)
	FString PathToLobby;
};
