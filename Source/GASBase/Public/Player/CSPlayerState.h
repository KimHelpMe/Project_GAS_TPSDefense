// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"


#include "CSPlayerState.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

// SelectedCharacterClass가 실제로 리플리케이트되어 도착했을 때 통지 (위젯이 Bind Event로 구독)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterSelected);

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

	// SelectedCharacterClass 리플리케이트 도착 시 호출 (도착 시점이 위젯 생성 시점보다 늦을 수 있어 OnCharacterSelected로 통지)
	UFUNCTION()
	void OnRep_SelectedCharacterClass();

public:
	UFUNCTION(BlueprintPure, Category = "GAS")
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
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SelectedCharacterClass)
	TSubclassOf<APawn> SelectedCharacterClass;


	// TargetPlayerState의 Bind Event 노드에서 구독해서, 늦게 도착한 캐릭터 선택 정보로 아이콘 등을 갱신할 때 사용
	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnCharacterSelected OnCharacterSelected;


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
