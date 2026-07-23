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

	void SetRespawnTime(float InTime) { RespawnTime = InTime; }

public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	float RespawnTime = 0.f;

	// 로비에서 선택한 캐릭터 클래스 (SeamlessTravel 후에도 유지됨)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SelectedCharacterClass)
	TSubclassOf<APawn> SelectedCharacterClass;

	// TargetPlayerState의 Bind Event 노드에서 구독해서, 늦게 도착한 캐릭터 선택 정보로 아이콘 등을 갱신할 때 사용
	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnCharacterSelected OnCharacterSelected;



private:
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};
