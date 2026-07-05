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
    Front,      // Forward 였으나, 파라곤 애니메이션 명이 Front라서 Front로 바꿈
    Back
};



UCLASS()
class GASBASE_API UCSBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
    // Dot Product / Cross Product로 피격 방향을 계산
    UFUNCTION(BlueprintPure)
    static EHitDirection GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator);

    // 계산된 방향을 몽타주 섹션 이름(FName)으로 변환
    UFUNCTION(BlueprintPure)
    static FName GetHitDirectionName(const EHitDirection& HitDirection);
};
