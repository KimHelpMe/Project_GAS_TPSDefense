// CSK

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CSBlueprintFunctionLibrary.generated.h"

// 피격 방향을 나타내는 열거형 (블루프린트 노출을 위해 uint8 기반 필요)
UENUM(BlueprintType)
enum class EHitDirection : uint8
{
    Left,
    Right,
    Forward,
    Back
};



UCLASS()
class GASBASE_API UCSBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};
