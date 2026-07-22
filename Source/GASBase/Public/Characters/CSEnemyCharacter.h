// CSK

#pragma once

#include "CoreMinimal.h"
#include "Characters/CSBaseCharacter.h"
#include "CSEnemyCharacter.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

UCLASS()
class GASBASE_API ACSEnemyCharacter : public ACSBaseCharacter
{
	GENERATED_BODY()

public:
	ACSEnemyCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	bool IsBoss() const { return bIsBoss; }

protected:
	virtual void BeginPlay() override;
	virtual UAttributeSet* GetAttributeSet() const override;

	// 적 전용: Health ≤ 0 시 직접 HandleDeath 호출
	virtual void OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData) override;

	// 보스 사망(복제된 bAlive 변경) 시 모든 클라이언트에서 보스 체력바 UI 숨김
	virtual void OnRep_Alive() override;

	// HandleDeath는 서버에서만 호출되며 OnRep은 서버(호스트) 자신에게는 오지 않으므로,
	// 서버/호스트 로컬 UI를 숨기기 위해 별도로 오버라이드
	virtual void HandleDeath_Implementation() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	// 보스 캐릭터 여부 — true면 스폰 시 화면 고정 보스 체력바 UI를 표시
	UPROPERTY(EditAnywhere, Category = "Boss")
	bool bIsBoss = false;
};
