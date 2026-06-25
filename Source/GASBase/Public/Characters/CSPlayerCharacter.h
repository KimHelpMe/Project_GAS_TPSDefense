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

protected:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

};
