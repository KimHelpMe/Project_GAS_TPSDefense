// CSK

#pragma once

#include "CoreMinimal.h"
#include "Characters/CSBaseCharacter.h"
#include "CSPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class GASBASE_API ACSPlayerCharacter : public ACSBaseCharacter
{
	GENERATED_BODY()

public:
	ACSPlayerCharacter();

	//~ Begin IAbilityInterface Interface 
	// PlayerState로부터 ASC를 가져와 반환
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilityInterface Interface

	virtual UAttributeSet* GetAttributeSet() const override;

	//~ Begin Character Interface
	// 서버: 빙의 시 ASC Owner/Avatar 초기화
	virtual void PossessedBy(AController* NewController) override;
	//~ End Character Interface

	//~ Begin Pawn Interface
	// 클라이언트: PlayerState 복제 시 ASC Owner/Avatar 초기화
	virtual void OnRep_PlayerState() override;
	//~ End Pawn Interface

private:
	// ASC 초기화 완료 후 어트리뷰트 변경 델리게이트를 바인딩한다.
	// 서버: PossessedBy, 클라이언트: OnRep_PlayerState에서 각각 호출
	void BindAttributeDelegates();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

};
