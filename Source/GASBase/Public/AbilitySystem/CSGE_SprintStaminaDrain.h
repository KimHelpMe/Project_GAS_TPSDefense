// CSK

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CSGE_SprintStaminaDrain.generated.h"

// 스프린트(Status.Sprinting 태그) 중 주기적으로 Stamina를 깎는 Infinite GE
UCLASS()
class GASBASE_API UCSGE_SprintStaminaDrain : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UCSGE_SprintStaminaDrain();
};
