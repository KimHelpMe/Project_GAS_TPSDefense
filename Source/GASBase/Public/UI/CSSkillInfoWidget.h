// CSK

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CSSkillInfoWidget.generated.h"

class UCSAbilityData;

// AbilityPickup 위에 떠 있는 스킬 정보 팝업의 C++ 베이스.
// 실제 아이콘/텍스트 배치는 이 클래스를 부모로 하는 위젯 블루프린트(WBP_SkillInfoPopup)에서 담당.
UCLASS()
class GASBASE_API UCSSkillInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// AssignedAbility가 정해지거나 리플리케이트로 갱신될 때 C++에서 호출.
	// 블루프린트에서 이 이벤트를 구현해 아이콘/이름/설명 텍스트를 채운다.
	UFUNCTION(BlueprintImplementableEvent, Category = "Skill Info")
	void Setup(UCSAbilityData* AbilityData);
};
