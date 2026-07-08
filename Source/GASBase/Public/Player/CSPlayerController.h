// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "CSPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class GASBASE_API ACSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void SetupInputComponent() override;

private:
	void ActivateAbility(const FGameplayTag& AbilityTag) const;

	// 클라이언트에서 로컬로 SendGameplayEventToActor를 호출하면 서버까지 자동으로 전달되지 않는다.
	// (Prediction Key 윈도우가 짧은 시간 안에 끝나는 경우에만 우연히 맞아떨어짐)
	// 그래서 서버로 확실하게 이벤트를 전달하기 위한 RPC.
	UFUNCTION(Server, Reliable)
	void ServerSendGameplayEvent(FGameplayTag EventTag);

	// Callback Movement Func
	void Jump();
	void StopJumping();
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Roll();

	// Callback Ability Func
	void LMB();
	void RMB();

	void Q();
	void E();
	void R();
	void T();

	// 현재 빙의된 폰의 생존 여부를 확인하는 헬퍼 함수
	bool IsAlive() const;

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

	UPROPERTY(EditDefaultsOnly, Category ="Input|Movement")
	TObjectPtr<UInputAction> RollAction;

	// Ability
	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> PrimaryAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> SecondaryAction;

	// Q
	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> QAction;

	// E
	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> EAction;

	// R
	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> RAction;

	// T
	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> TAction;
};
