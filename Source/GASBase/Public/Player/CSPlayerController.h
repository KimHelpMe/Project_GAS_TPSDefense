// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CSPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

struct FGameplayTag;

UCLASS()
class GASBASE_API ACSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void SetupInputComponent() override;

private:
	void ActivateAbility(const FGameplayTag& AbilityTag) const;

	// Callback Movement Func
	void Jump();
	void StopJumping();
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	// Callback Ability Func
	void Primary();
	void Secondary();
	void Tertiary();

private:
	// Movement
	UPROPERTY(EditDefaultsOnly, Category = "Input|MappingContext")
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Movement")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Movement")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Movement")
	TObjectPtr<UInputAction> LookAction;

	// Ability
	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> PrimaryAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> SecondaryAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> TertiaryAction;
};
