

#include "Game/InGameMode.h"
#include "Characters/CSBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameplayTags/CSGameplayTags.h"
#include "Player/CSPlayerState.h"
#include "Player/CSBasePlayerController.h"
#include "Player/CSPlayerController.h"
#include "Kismet/GameplayStatics.h"

AInGameMode::AInGameMode()
{
	bUseSeamlessTravel = true;
}

void AInGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	if (ACSBasePlayerController* PC = Cast<ACSBasePlayerController>(C))
	{
		PC->Client_EndBattleTransition();

		// SeamlessTravel 시 PC가 재활용되므로 BeginPlay가 호출되지 않음 — 입력 상태를 명시적으로 복구
		if (ACSPlayerController* CSPlayer = Cast<ACSPlayerController>(PC))
		{
			CSPlayer->Client_RestoreInput();
		}
	}
}

void AInGameMode::OnBossDied()
{
	// 중복 호출 방지 (에너미 헬스 0 이하 연습 발생 방지)
	if (bVictoryTriggered) return;
	bVictoryTriggered = true;

	// 모든 PlayerController에게 개별 통계 수집 후 승리 RPC 전송
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACSPlayerController* PC = Cast<ACSPlayerController>(It->Get());
		if (!PC) continue;

		FCSMatchResult Result;
		if (ACSPlayerState* PS = PC->GetPlayerState<ACSPlayerState>())
		{
			Result.KillCount = PS->KillCount;
			Result.TotalDamageDealt = PS->TotalDamageDealt;
		}

		PC->Client_NotifyVictory(VictoryLobbyDelay, Result);
	}

	// N초 후 로비로 자동 이동
	FTimerHandle VictoryTimer;
	GetWorldTimerManager().SetTimer(
		VictoryTimer,
		this,
		&ThisClass::ReturnAllPlayersToLobby,
		VictoryLobbyDelay,
		false
	);
}

void AInGameMode::ReturnAllPlayersToLobby()
{
	// 기존 ServerRequestReturnToLobby와 동일한 패턴
	UGameplayStatics::OpenLevel(this, FName("/MultiplayerSessions/Maps/Lobby"), true);
}

void AInGameMode::OnCharacterDied(AController* DeadController)
{
    if (!IsValid(DeadController)) return;

    // 이미 타이머가 걸려있으면 무시 (이중 사망 방지)
    if (FTimerHandle* Existing = RespawnTimers.Find(TWeakObjectPtr<AController>(DeadController)))
    {
        if (GetWorldTimerManager().IsTimerActive(*Existing)) return;
    }

    FTimerHandle& Handle = RespawnTimers.FindOrAdd(DeadController);
    FTimerDelegate Delegate;
    Delegate.BindUObject(this, &ThisClass::RespawnCharacter, DeadController);
    GetWorldTimerManager().SetTimer(Handle, Delegate, RespawnDelay, false);
    
    ACSPlayerState* PlayerController = DeadController->GetPlayerState<ACSPlayerState>();
    if (PlayerController)
    {
        // duration만 저장 — 클라이언트는 복제받은 시점(OnRep_RespawnTime)부터 자체 카운트다운
        PlayerController->SetRespawnTime(RespawnDelay);
    }
}

UClass* AInGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
    if (APlayerController* Controller = Cast<APlayerController>(InController))
    {
        if (ACSPlayerState* State = Controller->GetPlayerState<ACSPlayerState>())
        {
            if (State->SelectedCharacterClass)
            {
                return State->SelectedCharacterClass;
            }
        }
    }

    return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AInGameMode::RespawnCharacter(AController* Controller)
{
    RespawnTimers.Remove(TWeakObjectPtr<AController>(Controller));

    if (!IsValid(Controller)) return;

    APawn* Pawn = Controller->GetPawn();
    ACSBaseCharacter* Character = Cast<ACSBaseCharacter>(Pawn);
    if (!IsValid(Character)) return;

    // GA_Death 강제 종료 → Status.Dead 해제, GE_Death 제거
    UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
    if (IsValid(ASC))
    {
        FGameplayTagContainer DeathTag;
        DeathTag.AddTag(CSGameplayTags::Abilities::Death);
        ASC->CancelAbilities(&DeathTag);
    }

    // 어트리뷰트 리셋 (Health, Stamina 등 최대값으로)
    Character->ResetAttributes();

    // PlayerStart 위치로 텔레포트
    AActor* StartSpot = FindPlayerStart(Controller);
    if (IsValid(StartSpot))
    {
        Character->SetActorLocation(StartSpot->GetActorLocation());
        Character->SetActorRotation(StartSpot->GetActorRotation());
    }

    // HandleRespawn — bAlive=true, 충돌 복원
    Character->HandleRespawn();

    ACSPlayerState* PlayerController = Controller->GetPlayerState<ACSPlayerState>();
    if (PlayerController)
    {
        PlayerController->SetRespawnTime(0.f);
    }
}
