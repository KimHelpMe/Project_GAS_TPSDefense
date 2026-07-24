// CSK


#include "Player/CSPlayerState.h"
#include "AbilitySystem/CSAbilitySystemComponent.h"
#include "AbilitySystem/CSAttributeSet.h"
#include "Net/UnrealNetwork.h"

ACSPlayerState::ACSPlayerState()
{
	// 기본값 1Hz는 어빌리티 동기화에 너무 느리므로 100Hz로 설정
	SetNetUpdateFrequency(100);

	// ASC 생성 및 복제 설정
	AbilitySystemComponent = CreateDefaultSubobject<UCSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// 복제 모드: Mixed (플레이어 컨트롤 캐릭터에 적합한 설정)
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	/**  GAS의 자동 등록 메커니즘
	* CreateDefaultSubobject로 UAttributeSet 파생 오브젝트를 Outer(PlayerState)에 생성하면, 
	* GAS는 이 Outer에 붙어있는 ASC를 찾아 Attribute Set을 자동으로 등록한다. 
	* 개발자가 별도로 RegisterAttributeSet() 같은 함수를 호출할 필요가 없다.
	*/
	AttributeSet = CreateDefaultSubobject<UCSAttributeSet>(TEXT("AttributeSet"));
}

void ACSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, RespawnTime);
	DOREPLIFETIME(ThisClass, KillCount);
	DOREPLIFETIME(ThisClass, TotalDamageDealt);
	DOREPLIFETIME(ACSPlayerState, SelectedCharacterClass);
	DOREPLIFETIME(ACSPlayerState, SelectedCharacterID);
}

void ACSPlayerState::OnRep_RespawnTime()
{
	if (RespawnTime > 0.f)
	{
		// 복제받은 순간 로컬 기준점 저장
		LocalRespawnStartTime = GetWorld()->GetTimeSeconds();
		LocalRespawnDuration = RespawnTime;
	}
}


void ACSPlayerState::SetRespawnTime(float InDuration)
{
	RespawnTime = InDuration;

	// 서버는 OnRep이 호출되지 않으므로 직접 로컬 기준점 세팅
	if (InDuration > 0.f)
	{
		LocalRespawnStartTime = GetWorld()->GetTimeSeconds();
		LocalRespawnDuration = InDuration;
	}
}

void ACSPlayerState::SeamlessTravelTo(APlayerState* NewPlayerState)
{
	Super::SeamlessTravelTo(NewPlayerState);

	if (ACSPlayerState* NewState = Cast<ACSPlayerState>(NewPlayerState))
	{
		// 로비에서 선택한 캐릭터 클래스 + ID를 새 PlayerState로 이전
		NewState->SelectedCharacterClass = SelectedCharacterClass;
		NewState->SelectedCharacterID = SelectedCharacterID;
	}
}

void ACSPlayerState::ServerSelectCharacter_Implementation(TSubclassOf<APawn> InCharacterClass, ECSCharacterID InCharacterID)
{
	SelectedCharacterClass = InCharacterClass;
	SelectedCharacterID = InCharacterID;
}

void ACSPlayerState::OnRep_SelectedCharacterClass()
{
	OnCharacterSelected.Broadcast();
}

UAbilitySystemComponent* ACSPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
