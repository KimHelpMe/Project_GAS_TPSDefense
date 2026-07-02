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

protected:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

};
