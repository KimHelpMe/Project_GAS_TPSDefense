// CSK


#include "GameplayTags/CSGameplayTags.h"


namespace CSGameplayTags
{
	namespace Input
	{
		UE_DEFINE_GAMEPLAY_TAG(Q, "CSGameplayTags.Input.Q");
		UE_DEFINE_GAMEPLAY_TAG(E, "CSGameplayTags.Input.E");
		UE_DEFINE_GAMEPLAY_TAG(R, "CSGameplayTags.Input.R");
		UE_DEFINE_GAMEPLAY_TAG(T, "CSGameplayTags.Input.T");
		UE_DEFINE_GAMEPLAY_TAG(G, "CSGameplayTags.Input.G");
	}

	namespace Abilities
	{
		UE_DEFINE_GAMEPLAY_TAG(ActivateOnGiven, "CSGameplayTags.Abilities.ActivateOnGiven");

		UE_DEFINE_GAMEPLAY_TAG(Primary, "CSGameplayTags.Abilities.Primary");
		UE_DEFINE_GAMEPLAY_TAG(Secondary, "CSGameplayTags.Abilities.Secondary");
		UE_DEFINE_GAMEPLAY_TAG(Roll, "CSGameplayTags.Abilities.Roll");

		UE_DEFINE_GAMEPLAY_TAG(Death, "CSGameplayTags.Abilities.Death");


		//UE_DEFINE_GAMEPLAY_TAG(Q, "CSGameplayTags.Abilities.Q");
		//UE_DEFINE_GAMEPLAY_TAG(E, "CSGameplayTags.Abilities.E");
		//UE_DEFINE_GAMEPLAY_TAG(R, "CSGameplayTags.Abilities.R");
		//UE_DEFINE_GAMEPLAY_TAG(T, "CSGameplayTags.Abilities.T");
		//UE_DEFINE_GAMEPLAY_TAG(G, "CSGameplayTags.Abilities.G");

		UE_DEFINE_GAMEPLAY_TAG(BulletParty, "CSGameplayTags.Abilities.BulletParty");
		UE_DEFINE_GAMEPLAY_TAG(FireWall, "CSGameplayTags.Abilities.FireWall");
		UE_DEFINE_GAMEPLAY_TAG(SpawnSword, "CSGameplayTags.Abilities.SpawnSword");
		UE_DEFINE_GAMEPLAY_TAG(SpawnAxe, "CSGameplayTags.Abilities.SpawnAxe");
		UE_DEFINE_GAMEPLAY_TAG(FireSpawnSword, "CSGameplayTags.Abilities.FireSpawnSword");
		UE_DEFINE_GAMEPLAY_TAG(FireSpawnAxe, "CSGameplayTags.Abilities.FireSpawnAxe");
		UE_DEFINE_GAMEPLAY_TAG(Teleport, "CSGameplayTags.Abilities.Teleport");
		UE_DEFINE_GAMEPLAY_TAG(Laser, "CSGameplayTags.Abilities.Laser");


		UE_DEFINE_GAMEPLAY_TAG(TestAbility, "CSGameplayTags.Abilities.TestAbility");


		UE_DEFINE_GAMEPLAY_TAG(Fireball, "CSGameplayTags.Abilities.Fireball");

		UE_DEFINE_GAMEPLAY_TAG(Long_Primary, "CSGameplayTags.Abilities.Long_Primary");
		UE_DEFINE_GAMEPLAY_TAG(FireSpawnedWeapons, "CSGameplayTags.Abilities.FireSpawnedWeapons");




		UE_DEFINE_GAMEPLAY_TAG(Ray, "CSGameplayTags.Abilities.Ray");


		UE_DEFINE_GAMEPLAY_TAG(Sprint, "CSGameplayTags.Abilities.Sprint");

	}

	namespace Events
	{
		UE_DEFINE_GAMEPLAY_TAG(HitReact, "CSGameplayTags.Events.HitReact");
		UE_DEFINE_GAMEPLAY_TAG(Scored, "CSGameplayTags.Events.Scored");
		UE_DEFINE_GAMEPLAY_TAG(DamageDealt, "CSGameplayTags.Events.DamageDealt");

		UE_DEFINE_GAMEPLAY_TAG(SpawnProjectile, "CSGameplayTags.Events.SpawnProjectile");

		UE_DEFINE_GAMEPLAY_TAG(SpawnWeapon, "CSGameplayTags.Events.SpawnWeapon");
		UE_DEFINE_GAMEPLAY_TAG(SpawnFireWall, "CSGameplayTags.Events.SpawnFireWall");
		UE_DEFINE_GAMEPLAY_TAG(SpawnLaser, "CSGameplayTags.Events.SpawnLaser");

		UE_DEFINE_GAMEPLAY_TAG(SpawnAbilityBox, "CSGameplayTags.Events.SpawnAbilityBox");

		
		UE_DEFINE_GAMEPLAY_TAG(Ray, "CSGameplayTags.Events.SpawnRay");
	}


	// 주의: 반드시 GameplayCue.로 시작해야 GAS가 Cue로 인식
	namespace Cues
	{
		// 격발
		UE_DEFINE_GAMEPLAY_TAG(Fire_LMB_LtBelica, "GameplayCue.Fire.LMB.LtBelica");
		UE_DEFINE_GAMEPLAY_TAG(Fire_LMB_Murdock, "GameplayCue.Fire.LMB.Murdock");

		// 히트
		UE_DEFINE_GAMEPLAY_TAG(Impact_LMB_LtBelica, "GameplayCue.Impact.LMB.LtBelica");
		UE_DEFINE_GAMEPLAY_TAG(Impact_LMB_Murdock, "GameplayCue.Impact.LMB.Murdock");

		UE_DEFINE_GAMEPLAY_TAG(Impact_E, "GameplayCue.Impact.E");
		UE_DEFINE_GAMEPLAY_TAG(Impact_Fire, "GameplayCue.Impact.Fireball");
		UE_DEFINE_GAMEPLAY_TAG(Impact_Earthquake, "GameplayCue.Impact.Earthquake");
		UE_DEFINE_GAMEPLAY_TAG(Impact_Teleport, "GameplayCue.Impact.Teleport");


		// 트레이서 이펙트
		UE_DEFINE_GAMEPLAY_TAG(Tracer_LMB_LtBelica, "GameplayCue.Tracer.LMB.LtBelica");

		// 힐링 이펙트
		UE_DEFINE_GAMEPLAY_TAG(Effect_Healing, "GameplayCue.Effect.Healing");

		
	}

	namespace Status
	{
		UE_DEFINE_GAMEPLAY_TAG(Dead, "CSGameplayTags.Status.Dead");
		UE_DEFINE_GAMEPLAY_TAG(Casting, "CSGameplayTags.Status.Casting");
		UE_DEFINE_GAMEPLAY_TAG(WeaponsSpawned, "CSGameplayTags.Status.WeaponsSpawned");

		UE_DEFINE_GAMEPLAY_TAG(UsingStamina, "CSGameplayTags.Status.UsingStamina");

		UE_DEFINE_GAMEPLAY_TAG(Teleporting, "CSGameplayTags.Status.Teleporting");
		
		UE_DEFINE_GAMEPLAY_TAG(Sprinting, "CSGameplayTags.Status.Sprinting");
		UE_DEFINE_GAMEPLAY_TAG(Rolling, "CSGameplayTags.Status.Rolling");

	}

	namespace Cooldown
	{
		UE_DEFINE_GAMEPLAY_TAG(Fireball, "CSGameplayTags.Cooldown.Fireball");
		UE_DEFINE_GAMEPLAY_TAG(FireWall, "CSGameplayTags.Cooldown.FireWall");

		UE_DEFINE_GAMEPLAY_TAG(SpawnWeapon_Axe, "CSGameplayTags.Cooldown.SpawnWeapon.Axe");
		UE_DEFINE_GAMEPLAY_TAG(SpawnWeapon_Sword, "CSGameplayTags.Cooldown.SpawnWeapon.Sword");

		UE_DEFINE_GAMEPLAY_TAG(BulletParty, "CSGameplayTags.Cooldown.BulletParty");
		UE_DEFINE_GAMEPLAY_TAG(Teleport, "CSGameplayTags.Cooldown.Teleport");
		UE_DEFINE_GAMEPLAY_TAG(Laser, "CSGameplayTags.Cooldown.Laser");
	}

	namespace Data
	{
		UE_DEFINE_GAMEPLAY_TAG(Damage_Base, "CSGameplayTags.Data.Damage.Base");
		UE_DEFINE_GAMEPLAY_TAG(Damage_Distance, "CSGameplayTags.Data.Damage.Distance");
		UE_DEFINE_GAMEPLAY_TAG(Heal_Amount, "CSGameplayTags.Data.Heal.Amount");
	}

	namespace Team
	{
		UE_DEFINE_GAMEPLAY_TAG(Player, "CSGameplayTags.Team.Player");
		UE_DEFINE_GAMEPLAY_TAG(Enemy, "CSGameplayTags.Team.Enemy");
	}
}