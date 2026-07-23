// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"


#include "CSPlayerState.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class GASBASE_API ACSPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ACSPlayerState();

	//~ Begin IAbilitySystemInterface Interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface Interface

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// SeamlessTravel 시 커스텀 프로퍼티를 새 PlayerState로 복사
	virtual void SeamlessTravelTo(APlayerState* NewPlayerState) override;

	// 클라이언트 → 서버: 캐릭터 선택 저장
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Lobby")
	void ServerSelectCharacter(TSubclassOf<APawn> InCharacterClass);

public:
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	void SetRespawnTime(float InDuration);

	// 킬 수 누적 (서버에서 GA_ListenForScored 경유 호출)
	UFUNCTION(BlueprintCallable)
	void AddKillScore() { KillCount++; }

	// 데미지 누적 (서버에서 GA_ListenForScored 경유 호출)
	UFUNCTION(BlueprintCallable)
	void AddDamageScore(float Damage) { TotalDamageDealt += Damage; }

public:
	// 리스폰까지 남은 시간 (duration). 0이면 살아있음
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_RespawnTime)
	float RespawnTime = 0.f;

	// 이번 매치에서 처치한 적 수
	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 KillCount = 0;

	// 이번 매치에서 가한 총 데미지
	UPROPERTY(BlueprintReadOnly, Replicated)
	float TotalDamageDealt = 0.f;

	// 로비에서 선택한 캐릭터 클래스 (SeamlessTravel 후에도 유지됨)
	UPROPERTY(BlueprintReadOnly, Replicated)
	TSubclassOf<APawn> SelectedCharacterClass;

	// 클라이언트 전용 getter
	float GetLocalRespawnStartTime() const { return LocalRespawnStartTime; }
	float GetLocalRespawnDuration() const { return LocalRespawnDuration; }

private:
	// 클라이언트 전용 — 복제받은 시점의 로컬 TimeSeconds
	float LocalRespawnStartTime = 0.f;

	// 클라이언트 전용 — 복제받은 시점의 총 duration
	float LocalRespawnDuration = 0.f;

	UFUNCTION()
	void OnRep_RespawnTime();

	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};
