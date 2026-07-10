// CSK


#include "GameplayTags/CSGameplayTags.h"


namespace CSGameplayTags
{
	namespace Input
	{

	}

	namespace Abilities
	{
		UE_DEFINE_GAMEPLAY_TAG(ActivateOnGiven, "CSGameplayTags.Abilities.ActivateOnGiven");

		UE_DEFINE_GAMEPLAY_TAG(Primary, "CSGameplayTags.Abilities.Primary");
		UE_DEFINE_GAMEPLAY_TAG(Secondary, "CSGameplayTags.Abilities.Secondary");
		UE_DEFINE_GAMEPLAY_TAG(Roll, "CSGameplayTags.Abilities.Roll");

		UE_DEFINE_GAMEPLAY_TAG(Death, "CSGameplayTags.Abilities.Death");


		UE_DEFINE_GAMEPLAY_TAG(Q, "CSGameplayTags.Abilities.Q");
		UE_DEFINE_GAMEPLAY_TAG(E, "CSGameplayTags.Abilities.E");
		UE_DEFINE_GAMEPLAY_TAG(R, "CSGameplayTags.Abilities.R");

		UE_DEFINE_GAMEPLAY_TAG(G, "CSGameplayTags.Abilities.G");
		UE_DEFINE_GAMEPLAY_TAG(T, "CSGameplayTags.Abilities.T");


		UE_DEFINE_GAMEPLAY_TAG(Fireball, "CSGameplayTags.Abilities.Fireball");

		UE_DEFINE_GAMEPLAY_TAG(Long_Primary, "CSGameplayTags.Abilities.Long_Primary");
		UE_DEFINE_GAMEPLAY_TAG(FireSpawnedWeapons, "CSGameplayTags.Abilities.FireSpawnedWeapons");

	}

	namespace Events
	{
		UE_DEFINE_GAMEPLAY_TAG(HitReact, "CSGameplayTags.Events.HitReact");
		UE_DEFINE_GAMEPLAY_TAG(Scored,"CSGameplayTags.Events.Scored");

		UE_DEFINE_GAMEPLAY_TAG(SpawnProjectile, "CSGameplayTags.Events.SpawnProjectile");
		UE_DEFINE_GAMEPLAY_TAG(SpawnAbilityBox, "CSGameplayTags.Events.SpawnAbilityBox");

		

	}

	namespace Cues
	{
		// 주의: 반드시 GameplayCue.로 시작해야 GAS가 Cue로 인식
		UE_DEFINE_GAMEPLAY_TAG(Impact_E, "GameplayCue.Impact.E");
		UE_DEFINE_GAMEPLAY_TAG(Impact_Fire, "GameplayCue.Impact.Fireball");
		UE_DEFINE_GAMEPLAY_TAG(Impact_Earthquake, "GameplayCue.Impact.Earthquake");

	}

	namespace Status
	{
		UE_DEFINE_GAMEPLAY_TAG(Dead, "CSGameplayTags.Status.Dead");
		UE_DEFINE_GAMEPLAY_TAG(Casting, "CSGameplayTags.Status.Casting");
		UE_DEFINE_GAMEPLAY_TAG(WeaponsSpawned, "CSGameplayTags.Status.WeaponsSpawned");

		UE_DEFINE_GAMEPLAY_TAG(UsingStamina, "CSGameplayTags.Status.UsingStamina");

	}

	namespace Cooldown
	{
		UE_DEFINE_GAMEPLAY_TAG(Fireball, "CSGameplayTags.Cooldown.Fireball");
		UE_DEFINE_GAMEPLAY_TAG(FireWall, "CSGameplayTags.Cooldown.FireWall");

	}
}