// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CSGE_Heal.generated.h"

// 힐링존 등에서 사용하는 Infinite/Periodic 회복 GE
// 회복량은 SetByCaller(CSGameplayTags::Data::Heal_Amount)로 적용 시점에 전달받음
UCLASS()
class GASBASE_API UCSGE_Heal : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UCSGE_Heal();

	// GE의 Period와 동일한 값. 회복량(초당)을 틱당 값으로 환산할 때 호출부에서 참조
	static constexpr float TickInterval = 0.2f;
};
