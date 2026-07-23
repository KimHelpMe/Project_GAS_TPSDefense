

#include "Game/InGameMode.h"
#include "Characters/CSBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameplayTags/CSGameplayTags.h"
#include "Player/CSPlayerState.h"
#include "Player/CSPlayerController.h"

AInGameMode::AInGameMode()
{
	bUseSeamlessTravel = true;
}

void AInGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	if (ACSPlayerController* CSPlayer = Cast<ACSPlayerController>(C))
	{
		CSPlayer->Client_EndBattleTransition();
		CSPlayer->Client_RestoreInput(); // SeamlessTravel 시 PC가 재활용되므로 BeginPlay가 호출되지 않음 — 입력 상태를 명시적으로 복구
	}
}

void AInGameMode::OnCharacterDied(AController* DeadController)
{
    if (!IsValid(DeadController)) return;

    // 이미 타이머가 걸려있으면 무시 (이중 사망 방지)
    if (FTimerHandle* Existing = RespawnTimers.Find(DeadController))
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
        PlayerController->SetRespawnTime(GetWorld()->GetTimeSeconds() + RespawnDelay);
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
    RespawnTimers.Remove(Controller);

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
