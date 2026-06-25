// CSK


#include "Player/CSPlayerController.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"


void ACSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Enhanced Input 서브시스템 가져오기
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!IsValid(InputSubsystem)) return;

	// 배열의 모든 IMC를 우선순위 0으로 등록
	for (UInputMappingContext* IMC : InputMappingContexts)
	{
		InputSubsystem->AddMappingContext(IMC, 0);
	}

	// EnhancedInputComponent로 캐스팅
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EnhancedInputComponent)) return;

	// 입력 액션 → 콜백 바인딩
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::StopJumping);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Started, this, &ThisClass::Primary);
}

void ACSPlayerController::Jump()
{
	if (!IsValid(GetCharacter())) return;
	GetCharacter()->Jump();
}

void ACSPlayerController::StopJumping()
{
	if (!IsValid(GetCharacter())) return;
	GetCharacter()->StopJumping();
}

void ACSPlayerController::Move(const FInputActionValue& Value)
{
	if (!IsValid(GetPawn())) return;

	// 입력 값을 2D 벡터로 가져옴 
	const FVector2D MovementVector = Value.Get<FVector2D>();

	// 컨트롤러 Yaw만 (pitch, yaw, roll)
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

	// Yaw 회전으로부터 전방/우측 벡터 계산
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	GetPawn()->AddMovementInput(ForwardDirection, MovementVector.Y);
	GetPawn()->AddMovementInput(RightDirection, MovementVector.X);


}

void ACSPlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();

	AddYawInput(LookVector.X);
	AddPitchInput(LookVector.Y);
}

void ACSPlayerController::Primary()
{
	UE_LOG(LogTemp, Warning, TEXT("Primary Action"));
}
