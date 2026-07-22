// CSK

#pragma once

#include "CoreMinimal.h"
#include "Characters/CSBaseCharacter.h"
#include "CSPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ACSAbilityPickup;
class UCSCameraFadeComponent;
class AGameplayAbilityWorldReticle;

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
	//~ Begin Actor Interface
	virtual void BeginPlay() override;
	//~ End Actor Interface

	// 플레이어 전용 사망/리스폰 처리 (컨트롤러 회전, GameMode 통보)
	virtual void HandleDeath_Implementation() override;
	virtual void HandleRespawn() override;
	virtual void OnRep_Alive() override;

private:
	// ASC 초기화 완료 후 어트리뷰트 변경 델리게이트를 바인딩한다.
	// 서버: PossessedBy, 클라이언트: OnRep_PlayerState에서 각각 호출
	void BindAttributeDelegates();

	// Status.Sprinting 태그가 붙거나 떨어질 때 호출 — 스태미나 드레인 GE를 적용/제거 (서버 전용)
	void OnSprintingTagChanged(const FGameplayTag Tag, int32 NewCount);

	// 스프린트 중 적용된 스태미나 드레인 GE의 핸들 (해제 시 사용)
	FActiveGameplayEffectHandle SprintStaminaDrainHandle;

	// 크로스헤어(화면 중앙) 라인트레이스로 AbilityPickup을 조준 중인지 주기적으로 확인.
	// 로컬 컨트롤 캐릭터가 아니면 내부에서 조기 반환하므로, BeginPlay에서 무조건 타이머만 걸어둔다.
	void UpdateCrosshairTrace();

	// 현재 조준 중인 Pickup. 조준이 다른 곳으로 바뀌면 이전 것의 정보 위젯을 꺼야 하므로 기억해둠.
	UPROPERTY()
	TWeakObjectPtr<ACSAbilityPickup> HoveredPickup;

	// UpdateCrosshairTrace를 0.1초 간격으로 반복 실행시키는 타이머 핸들 (BeginPlay에서 등록)
	FTimerHandle CrosshairTraceTimerHandle;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;



public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	FGameplayTag GetFireSpawnedWeaponEventTag() const;

	//UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCSCameraFadeComponent> CameraFadeComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Teleport")
	TSubclassOf<AGameplayAbilityWorldReticle> TeleportReticleClass;
};
