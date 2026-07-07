// CSK

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

/*
* [네이티브 태그를 사용하는 이유]
* 블루프린트 태그 피커로 선택한 태그는 선택 시점엔 유효하지만, 
* 태그 이름이 바뀌어도 컴파일 에러가 발생하지 않아 추적이 어렵다. 
* 반면 네이티브 C++ 태그는 정의와 사용이 코드로 연결되어 있으므로, 
* 이름을 변경하면 컴파일 에러가 즉시 발생해 리팩토링이 안전하다.
*/


namespace CSGameplayTags
{
	namespace Abilities
	{
		// ASC에 부여(Give)되는 즉시 자동 활성화할 어빌리티에 붙이는 메타 태그
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ActivateOnGiven);

		// 좌클릭(LMB) — 근접 기본 공격 (SphereOverlap 히트박스)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary);
		// 우클릭(RMB) — 보조 공격 / 조준
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Secondary);
		// 회피 구르기
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Roll);

		// 사망 처리 어빌리티 — 체력 0 이하 시 활성화
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Death);

		// 스킬 슬롯 Q
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Q);
		// 스킬 슬롯 E
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(E);
		// 스킬 슬롯 R
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(R);

		// 스킬 슬롯 G
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(G);

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(T);

		// 파이어볼 투사체 어빌리티
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Fireball);
	}

	namespace Events
	{
		// 피격 반응 이벤트 — 공격자 정보를 담아 적에게 전송
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
		// 투사체 스폰 이벤트 — 몽타주 AnimNotify에서 발화
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpawnProjectile);

		// 점수 획득 이벤트
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Scored);
	}

	namespace Cues
	{
		// 투사체 충돌 시 이펙트/사운드를 재생하는 GameplayCue
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Impact_E);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Impace_Fire);
	}

	namespace Status
	{
		// 사망 상태 — 사망 중 다른 어빌리티 차단용 태그
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dead);
		// 캐스팅 중 — 스킬 시전 중 이동/공격 제한용 태그
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Casting);

	}

	namespace Cooldown
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Fireball);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireWall);
	}
}
