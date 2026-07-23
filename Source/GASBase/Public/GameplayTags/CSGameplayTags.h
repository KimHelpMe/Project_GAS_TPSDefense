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
	namespace Input
	{
		// 스킬 슬롯 Q
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Q);

		// 스킬 슬롯 E
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(E);

		// 스킬 슬롯 R
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(R);
		// 스킬 슬롯 T
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(T);

		// 스킬 슬롯 G
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(G);
	}
	
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

		//// 스킬 슬롯 Q
		//UE_DECLARE_GAMEPLAY_TAG_EXTERN(Q);
		//
		//// 스킬 슬롯 E
		//UE_DECLARE_GAMEPLAY_TAG_EXTERN(E);

		//// 스킬 슬롯 R
		//UE_DECLARE_GAMEPLAY_TAG_EXTERN(R);
		//// 스킬 슬롯 T
		//UE_DECLARE_GAMEPLAY_TAG_EXTERN(T);

		//// 스킬 슬롯 G
		//UE_DECLARE_GAMEPLAY_TAG_EXTERN(G);

		// BulletParty(난사) 어빌리티
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(BulletParty);
		// FireWall 구현 어빌리티
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireWall);
		// SpawnWeapon 어빌리티
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpawnSword);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpawnAxe);
		// FireSpawnWeapon 어빌리티
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireSpawnSword);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireSpawnAxe);
		// Teleport 어빌리티
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Teleport);
		// Laser 어빌리티
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Laser);

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TestAbility);

		// 파이어볼 투사체 어빌리티
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Fireball);

		// 원거리 기본 공격(GA_LMB) 내부 WaitGameplayEvent가 기다리는 트리거 태그
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Long_Primary);

		// GA_SpawnWeapon으로 소환된 무기를 발사시키는 전용 트리거 태그
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireSpawnedWeapons);


		// THE - Ray 어빌리티
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ray);

		// Sprint 어빌리티
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sprint);


	}

	namespace Events
	{
		// 피격 반응 이벤트 — 공격자 정보를 담아 적에게 전송
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);

		// 투사체 스폰 이벤트 — 몽타주 AnimNotify에서 발화
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpawnProjectile);
		// 무기 소환 이벤트
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpawnWeapon);

		// 장벽 소환 이벤트
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpawnFireWall);

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpawnLaser);

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpawnAbilityBox);

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ray);

		// 점수 획득 이벤트
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Scored);

		// 데미지 누적 이벤트 — 공격자의 총 데미지 통계 집계용
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(DamageDealt);
	}

	namespace Cues
	{
		// 격발 시 재생하는 GameplayCue
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Fire_LMB_LtBelica);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Fire_LMB_Murdock);

		// 투사체 충돌 시 이펙트/사운드를 재생하는 GameplayCue
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Impact_LMB_LtBelica);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Impact_LMB_Murdock);

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Impact_E);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Impact_Fire);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Impact_Earthquake);


		// 트레이서 이펙트
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tracer_LMB_LtBelica);

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Impact_Teleport);

		// 힐링 이펙트
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Healing);

	}

	namespace Status
	{
		// 사망 상태 — 사망 중 다른 어빌리티 차단용 태그
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dead);
		// 캐스팅 중 — 스킬 시전 중 이동/공격 제한용 태그
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Casting);
		// 무기 소환~발사 대기 중임을 나타내는 상태 태그 (평타 차단용)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponsSpawned);

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(UsingStamina);

		// 텔레포트 어빌리티가 targeting~confirm까지 실행 중임을 나타내는 상태 태그
		// (같은 입력 태그를 시작/확인 두 용도로 재사용하면서 생기는 자기 자신 트리거 문제를 막기 위해 사용)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Teleporting);

		// 달리고 있는 상태를 나타내는 태그
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sprinting);
	}

	namespace Cooldown
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Fireball);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireWall);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpawnWeapon_Axe);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpawnWeapon_Sword);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(BulletParty);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Teleport);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Laser);
	}

	namespace Data
	{
		// SetByCaller — 기본 데미지 수치
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Base);
		// SetByCaller — 트레이스 거리 (cm), Falloff 계산용
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Distance);
		// SetByCaller — 틱당 회복량 (HealingZone 등에서 사용)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Heal_Amount);
	}

	namespace Team
	{
		// 플레이어 팀 소속 — 이 태그가 붙은 대상에게는 플레이어 데미지 GE가 적용되지 않음
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player);
		// 에너미 팀 소속
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy);
	}
}
