// CSK

#include "Player/CSBasePlayerController.h"
#include "Chat/ChatComponent.h"
#include "UI/ChatWidget.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "Characters/CSBaseCharacter.h"

ACSBasePlayerController::ACSBasePlayerController()
{
	ChatComponent = CreateDefaultSubobject<UChatComponent>(TEXT("ChatComponent"));
}

void ACSBasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!IsValid(InputSubsystem)) return;

	for (UInputMappingContext* IMC : InputMappingContexts)
	{
		InputSubsystem->AddMappingContext(IMC, 0);
	}

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EnhancedInputComponent)) return;

	// 이동 입력
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::StopJumping);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

	// 채팅 활성화 (Enter)
	if (ChatActivateAction)
	{
		EnhancedInputComponent->BindAction(ChatActivateAction, ETriggerEvent::Started, this, &ThisClass::ActivateChat);
	}
}

void ACSBasePlayerController::ActivateChat()
{
	if (bChatActive) return;

	UChatWidget* Chat = GetChatWidget();
	if (!Chat) return;

	bChatActive = true;

	// 커서 활성 + UI 입력 허용
	bShowMouseCursor = true;
	FInputModeGameAndUI InputMode;
	SetInputMode(InputMode);

	// ChatWidget 활성 상태 진입 (입력창 표시 + 포커스 + 페이드 중단)
	Chat->ActivateChat();

	// 비활성 델리게이트 바인딩 (이미 바인딩되어 있으면 중복 방지)
	if (!Chat->OnChatDeactivateRequested.IsAlreadyBound(this, &ThisClass::DeactivateChat))
	{
		Chat->OnChatDeactivateRequested.AddDynamic(this, &ThisClass::DeactivateChat);
	}
}

void ACSBasePlayerController::DeactivateChat()
{
	if (!bChatActive) return;

	bChatActive = false;

	// 커서 비활성 + 게임 전용 입력
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void ACSBasePlayerController::Jump()
{
	if (bChatActive) return;
	if (!IsValid(GetCharacter())) return;
	if (!IsAlive()) return;
	GetCharacter()->Jump();
}

void ACSBasePlayerController::StopJumping()
{
	if (!IsValid(GetCharacter())) return;
	if (!IsAlive()) return;
	GetCharacter()->StopJumping();
}

void ACSBasePlayerController::Move(const FInputActionValue& Value)
{
	if (bChatActive) return;
	if (!IsValid(GetPawn())) return;
	if (!IsAlive()) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	GetPawn()->AddMovementInput(ForwardDirection, MovementVector.Y);
	GetPawn()->AddMovementInput(RightDirection, MovementVector.X);
}

void ACSBasePlayerController::Look(const FInputActionValue& Value)
{
	if (bChatActive) return;
	const FVector2D LookVector = Value.Get<FVector2D>();
	AddYawInput(LookVector.X);
	AddPitchInput(LookVector.Y);
}

bool ACSBasePlayerController::IsAlive() const
{
	ACSBaseCharacter* BaseCharacter = Cast<ACSBaseCharacter>(GetPawn());
	if (!IsValid(BaseCharacter)) return false;
	return BaseCharacter->IsAlive();
}
