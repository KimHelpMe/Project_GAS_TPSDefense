// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CSBasePlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UChatComponent;
class UChatWidget;
struct FInputActionValue;

// 이동 입력 + 채팅을 공유하는 베이스 PC
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

	// --- 채팅 활성/비활성 ---

	// Enter 키 입력 시 호출
	void ActivateChat();

	// ChatWidget에서 비활성 요청 시 호출 (전송 완료 / Escape)
	UFUNCTION()
	void DeactivateChat();

	// 하위 클래스가 오버라이드: 자신이 소유한 ChatWidget 반환
	virtual UChatWidget* GetChatWidget() const { return nullptr; }

	// 채팅 활성 상태
	bool bChatActive = false;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input|MappingContext")
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Movement")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Movement")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Movement")
	TObjectPtr<UInputAction> LookAction;

	// Enter 키: 채팅 활성화
	UPROPERTY(EditDefaultsOnly, Category = "Input|Chat")
	TObjectPtr<UInputAction> ChatActivateAction;

	UPROPERTY(VisibleAnywhere, Category = "Chat")
	TObjectPtr<UChatComponent> ChatComponent;
};
