// CSK


#include "UI/Menu.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

#include "Online/OnlineSessionNames.h"



bool UMenu::Initialize()
{
	if (!Super::Initialize() || !IsValid(HostButton) || !IsValid(JoinButton))
	{
		return false;
	}

	//  버튼 콜백 함수 바인딩
	HostButton->OnClicked.AddDynamic(this, &ThisClass::OnHostButtonClicked);
	JoinButton->OnClicked.AddDynamic(this, &ThisClass::OnJoinButtonClicked);

	return true;
}

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	// 블루프린트 인자 값을 내부 변수에 할당
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;

	// 입력받은 레벨 경로 뒤에 '?listen' 옵션을 붙여 최종 이동 주소를 조립합니다.
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);

	// 위젯을 화면에 추가하고 가시성 설정
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);

	// 위젯이 포커스를 받을 수 있도록 활성화
	bIsFocusable = true;

	// 월드 및 플레이어 컨트롤러 안전하게 가져오기
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			// UI 전용 입력 모드 구조체 선언 및 세부 설정
			FInputModeUIOnly InputModeData;

			// 이 위젯 자체에 Slate 포커스를 맞춤
			InputModeData.SetWidgetToFocus(TakeWidget());

			// 마우스 커서가 화면 밖으로 나갈 수 있도록 록 해제
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			// 컨트롤러에 입력 모드 적용 및 커서 시각화
			PlayerController->SetInputMode(InputModeData);
			PlayerController->bShowMouseCursor = true;
		}
	}

	// 게임 인스턴스로부터 커스텀 세션 서브시스템을 찾아 캐싱
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		// 템플릿 인자로 우리가 만든 서브시스템 클래스를 지정하여 가져옵니다.
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	// 서브시스템의 커스텀 델리게이트에 내 C++ 초기화 콜백 함수 동적 바인딩 설정
	if (MultiplayerSessionsSubsystem)
	{
		// 1. 다이나믹 멀티캐스트 델리게이트 연동 -> AddDynamic 사용
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);

		// 2. 일반 멀티캐스트 델리게이트 연동 (C++ 네이티브 에셋 인자용) -> AddUObject 사용
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
	}


}


// 위젯이 파괴되거나 레벨에서 제거될 때 엔진이 자동 호출
void UMenu::NativeDestruct()
{
	// 1. 우리가 만든 입력 모드 복구 로직 먼저 수행
	MenuTearDown();

	// 2. 부모 클래스의 기본 정리 함수를 맨 마지막에 호출 (중요)
	Super::NativeDestruct();
}

// 메뉴 해제 및 입력 상태 복구 구현부
void UMenu::MenuTearDown()
{
	// 위젯을 뷰포트(화면)에서 제거
	RemoveFromParent();

	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			// 입력 모드를 다시 '게임 전용'으로 전환 (캐릭터 조작 가능 상태로 변경)
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);

			// UI 전용 마우스 커서 숨기기
			PlayerController->bShowMouseCursor = false;
		}
	}
}

// 서브시스템이 세션 생성을 완료한 후 브로드캐스트하면 실행되는 구역
void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		if (HostButton) HostButton->SetIsEnabled(true);
		return;
	}

	// 세션이 완벽히 만들어진 것이 보장되었으므로, 안전하게 플러그인 맵 내부 로비 레벨로 이동
	if (UWorld* World = GetWorld())
	{
		// 플러그인 맵 최적화 경로 주입
		//World->ServerTravel(TEXT("/MultiplayerSessions/Map/Lobby?listen"));
		World->ServerTravel(PathToLobby);
	}
}

// 서브시스템이 방 검색 결과를 돌려주었을 때 실행되는 구역
void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (MultiplayerSessionsSubsystem == nullptr) return;

	// 성공했더라도 방 목록이 비어있으면 다시 누를 수 있게 열어주어야 합니다.
	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		if (JoinButton) JoinButton->SetIsEnabled(true);
		return;
	}

	// 탐색된 방 목록을 순회하며 매치 타입 필터링 검사
	for (const FOnlineSessionSearchResult& Result : SessionResults)
	{
		FString SettingsValue;
		// SETTING_MATCHTYPE 커스텀 키를 통해 데이터 추출
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);

		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow,
			FString::Printf(TEXT("Session MatchType: %s"), *SettingsValue));

		// 내가 설정한 매치 타입(예: FreeForAll)과 일치하는 방을 찾았다면
		if (SettingsValue == MatchType)
		{
			// 서브시스템에 해당 결과를 토대로 조인(방 입장) 비동기 요청 가동
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return; // 방을 찾았으므로 루프 즉시 탈출
		}
	}

	// 루프를 다 돌았는데 매칭되는 세션이 없었을 때
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red,
		TEXT("No matching session found!"));
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	// 조인 결과 자체가 실패한 경우 얼리 리턴
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("Failed to join session."));
		if (JoinButton) JoinButton->SetIsEnabled(true); // 버튼 복구
		return;
	}

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("Successfully joined session! Accessing address..."));

	// 주소 분석을 위해 로컬 세션 인터페이스 획득
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem) return;


	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	if (!SessionInterface.IsValid()) return;

	// 세션 연결 주소(IP/스팀 주소) 획득
	FString ConnectAddress;
	if (!SessionInterface->GetResolvedConnectString(NAME_GameSession, ConnectAddress))
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("Failed to get resolve connect string."));
		return;
	}

	// 컨트롤러 확보 및 이동
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance) return;

	APlayerController* PlayerController = GameInstance->GetFirstLocalPlayerController();
	if (!PlayerController) return;

	// ClientTravel을 통해 해당 주소의 리슨 서버에 게스트(클라이언트)로 원격 접속!
	PlayerController->ClientTravel(ConnectAddress, ETravelType::TRAVEL_Absolute);


}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("Failed to destroy session."));
		return;
	}

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("Session destroyed successfully."));

	// 버튼 다시 활성화하여 재호스팅/재검색 가능하게 복구
	if (HostButton) HostButton->SetIsEnabled(true);
	if (JoinButton) JoinButton->SetIsEnabled(true);
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("Session started successfully."));
	}
	else
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("Failed to start session."));
	}
}

void UMenu::OnHostButtonClicked()
{
	if (HostButton) HostButton->SetIsEnabled(false); // 스팸 클릭 방지

	// 캐싱된 서브시스템이 유효하다면 세션 생성 함수 작동 (아직 구현부는 비어있음)
	if (MultiplayerSessionsSubsystem)
	{
		// 설정된 세션 규칙 인자를 서브시스템에 전달하며 생성 요청
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMenu::OnJoinButtonClicked()
{
	if (JoinButton) JoinButton->SetIsEnabled(false); // 스팸 클릭 방지

	if (MultiplayerSessionsSubsystem)
	{
		// 10000개의 방을 최대로 탐색 요청 (스팀 앱ID free-pass 방 노이즈 극복용 고역치 설정)
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}