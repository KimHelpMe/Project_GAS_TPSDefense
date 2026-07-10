#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSAbilityPickup.generated.h"

class UCSAbilityData;
class USphereComponent;
class UStaticMeshComponent;
class UWidgetComponent;

UCLASS()
class GASBASE_API ACSAbilityPickup : public AActor
{
	GENERATED_BODY()

public:
	ACSAbilityPickup();

	// AssignedAbility(리플리케이트 프로퍼티)를 등록하기 위해 오버라이드
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 스폰 시점에 정해진 스킬 조회 (크로스헤어 호버 UI 등에서 사용)
	UFUNCTION(BlueprintPure, Category = "Pickup")
	UCSAbilityData* GetAssignedAbility() const { return AssignedAbility; }

	// 크로스헤어가 이 박스를 조준할 때/벗어날 때 정보 위젯 표시 여부를 전환
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetInfoWidgetVisible(bool bVisible);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

public:
	// 이 박스가 랜덤으로 뽑을 수 있는 스킬 후보군
	UPROPERTY(EditAnywhere, Category = "Pickup")
	TArray<TObjectPtr<UCSAbilityData>> PossibleAbilities;


	// Table을 사용하는 방식
#pragma region CSK/ DropSubsystem
	
	// true면 PossibleAbilities 대신 DropSubsystem에서 뽑기
	UPROPERTY(EditAnywhere, Category = "Pickup")
	bool bUseDropTable = false;

	// 현재 웨이브 (스폰 시 외부에서 세팅)
	UPROPERTY(BlueprintReadWrite, Category = "Pickup")
	int32 SpawnWave = 0;

#pragma endregion


private:
	// 스폰 시점에 랜덤으로 정해져 고정되는 실제 스킬. 습득 시 재추첨하지 않고 이 값을 그대로 사용.
	// 클라이언트가 크로스헤어 호버 UI에 내용을 표시하려면 이 값을 알아야 하므로 리플리케이트함.
	UPROPERTY(ReplicatedUsing = OnRep_AssignedAbility, VisibleInstanceOnly, Category = "Pickup")
	TObjectPtr<UCSAbilityData> AssignedAbility;

	// 클라이언트 전용 콜백: 서버가 정한 AssignedAbility가 복제로 도착했을 때 호출됨
	UFUNCTION()
	void OnRep_AssignedAbility();

	// AssignedAbility가 정해질 때(서버) / 리플리케이트로 도착할 때(클라이언트) 위젯 내용을 채움
	void UpdateInfoWidgetContent();

	// 실제 스폰박스 비주얼(메시, 파티클 등)은 블루프린트 자식(BP_AbilityPickup)에서.

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	TObjectPtr<USphereComponent> OverlapSphere;

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	TObjectPtr<UStaticMeshComponent> Mesh;

	// 박스 위에 떠서 스킬 정보를 보여주는 3D 월드스페이스 위젯. 기본은 숨김.
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	TObjectPtr<UWidgetComponent> InfoWidgetComponent;
};