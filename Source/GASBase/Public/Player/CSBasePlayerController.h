// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CSBasePlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UChatComponent;
struct FInputActionValue;

// 이동 입력 + 채팅 컴포넌트를 공유하는 베이스 PC
// ACSPlayerController(인게임)와 ACSLobbyPlayerController(로비) 모두 이 클래스를 상속
UCLASS()
class GASBASE_API ACSBasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACSBasePlayerController();

	FORCEINLINE UChatComponent* GetChatComponent() const { return ChatComponent; }

protected:
	virtual void SetupInputComponent() override;

	// 이동 콜백 (하위 클래스에서 오버라이드 가능)
	virtual void Jump();
	virtual void StopJumping();
	virtual void Move(const FInputActionValue& Value);
	virtual void Look(const FInputActionValue& Value);

	// 생존 여부 확인 헬퍼
	bool IsAlive() const;

protected:
	// IMC 배열 (에디터에서 할당)
	UPROPERTY(EditDefaultsOnly, Category = "Input|MappingContext")
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Movement")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Movement")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Movement")
	TObjectPtr<UInputAction> LookAction;

	// 채팅 컴포넌트 (로비/인게임 공용)
	UPROPERTY(VisibleAnywhere, Category = "Chat")
	TObjectPtr<UChatComponent> ChatComponent;
};
