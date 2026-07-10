// CSK


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))

{
	// 빌드 구성 환경에 맞는 온라인 서브시스템(예: Steam, NULL 등)
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();

	if (Subsystem)
	{
		// 서브시스템으로부터 세션 인터페이스 포인터 획득
		SessionInterface = Subsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			// UE 5.7 로깅 시스템을 활용한 디버깅 확인 로그 
			UE_LOG(LogTemp, Log, TEXT("Found Session Interface: %s"), *Subsystem->GetSubsystemName().ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find Online Subsystem!"));
	}
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 강제 종료 등으로 남아있는 좀비 세션을 정리
	if (SessionInterface.IsValid())
	{
		FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
		if (ExistingSession)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found leftover session on startup, destroying it."));
			SessionInterface->DestroySession(NAME_GameSession);
		}
	}
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
	// 종료 시 세션 정리
	if (SessionInterface.IsValid())
	{
		FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
		if (ExistingSession)
		{
			SessionInterface->DestroySession(NAME_GameSession);
		}
	}
	SessionInterface.Reset();
	Super::Deinitialize();
}

// 세션 생성
void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if (!SessionInterface.IsValid())
	{
		// 인터페이스가 무효하면 Menu에 즉시 실패(false) 알림
		MultiplayerOnCreateSessionComplete.Broadcast(false);
		return;
	}

	// 기존에 동일한 이름의 세션이 이미 존재한다면 먼저 파괴
	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		// 기존 세션이 있다면 플래그를 키고 설정을 캐싱
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;

		DestroySession();
		return;
	}

	// 델리게이트를 인터페이스 리스트에 등록하고, 추후 해제를 위한 핸들 저장
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// 세션 설정 객체 생성 (MakeShareable 사용)
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());

	// 서브시스템 종류 동적 체크 (Steam vs NULL)하여 LAN 매치 여부 결정
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		// NULL 서브시스템이면 LAN Match를 true로, Steam이면 false로 세팅
		LastSessionSettings->bIsLANMatch = (Subsystem->GetSubsystemName() == "NULL");
	}

	// 공용 매개변수 및 규칙 바인딩
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;

	// UE 5.x / 5.7 스팀 세션 연결 핵심 필수 설정 추가
	LastSessionSettings->bUseLobbiesIfAvailable = true;

	// 🛠️ 다른 개발자 빌드의 세션 무차별 혼선을 방지하기 위해 우리만의 고유 ID 설정 주입
	LastSessionSettings->BuildUniqueId = 1;

	// 매치 타입 키-값 쌍 추가 (예: FreeForAll 등)
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// 로컬 유저의 UniqueNetID를 획득하여 실제 세션 생성 함수 호출
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!LocalPlayer || !SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		// 만약 세션 생성 요청 자체가 실패했다면 델리게이트 핸들을 즉시 클리어
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

		// 요청 자체 실패 시 false 브로드캐스트
		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
}

// 세션 검색 
void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid()) return;

	// 엔진 OSI 델리게이트 맵에 검색 완료 핸들 바인딩
	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	// 검색 세팅 객체 빌드
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;

	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";

	// ⭐ UE 5.6 / 5.7 필수 핵심: Deprecated 된 SEARCH_PRESENCE 대신 SEARCH_LOBBIES 사용
	LastSessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!LocalPlayer || !SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		// 요청 자체 실패 시 즉시 핸들 해제 및 UI에 빈 배열/false 통보
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

// 세션 조인
void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	// 엔진 OSI 델리게이트 맵에 조인 완료 핸들 바인딩
	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!LocalPlayer || !SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}

	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		MultiplayerOnDestroySessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::StartSession()
{
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnStartSessionComplete.Broadcast(false);
		return;
	}

	StartSessionCompleteDelegateHandle = SessionInterface->AddOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegate);

	if (!SessionInterface->StartSession(NAME_GameSession))
	{
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);
		MultiplayerOnStartSessionComplete.Broadcast(false);
	}
}


// 비동기 응답 처리 콜백 내부
void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		// 사용이 끝난 엔진 OSI 델리게이트 핸들 해제
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	// 연동된 Menu UI 클래스들에게 최종 성공 여부 전송
	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

// 비동기 검색 완료 콜백
void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	// 검색 결과 배열이 비어있다면 실패 처리
	if (!LastSessionSearch.IsValid() || LastSessionSearch->SearchResults.Num() <= 0 || !bWasSuccessful)
	{
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	// UI(Menu) 측에 결과 목록 브로드캐스트 전송
	MultiplayerOnFindSessionsComplete.Broadcast(LastSessionSearch->SearchResults, true);
}

// 비동기 조인 완료 콜백
void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	// UI(Menu) 측에 최종 조인 결과 전송
	MultiplayerOnJoinSessionComplete.Broadcast(Result);
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	// 세션 파괴가 정상 성공했고, 재개설 플래그가 켜져있다면 저장해둔 옵션으로 안전하게 다시 개설 프로세스 진입
	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false; // 플래그 초기화
		CreateSession(LastNumPublicConnections, LastMatchType);
	}

	// 외부 UI에도 파괴 완료 브로드캐스트
	MultiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);
	}

	MultiplayerOnStartSessionComplete.Broadcast(bWasSuccessful);
}