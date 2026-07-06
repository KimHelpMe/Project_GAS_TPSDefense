// CSK


#include "GameplayTags/CSGameplayTags.h"


namespace CSGameplayTags
{
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

	}

	namespace Events
	{
		UE_DEFINE_GAMEPLAY_TAG(HitReact, "CSGameplayTags.Events.HitReact");
		UE_DEFINE_GAMEPLAY_TAG(Scored,"CSGameplayTags.Events.Scored");
	}

	namespace Cues
	{
		// 주의: 반드시 GameplayCue.로 시작해야 GAS가 Cue로 인식
		UE_DEFINE_GAMEPLAY_TAG(Projectile_Impact, "GameplayCue.Projectile.Impact");
	}

	namespace Status
	{
		UE_DEFINE_GAMEPLAY_TAG(Dead, "CSGameplayTags.Status.Dead");

	}
}