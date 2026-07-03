// CSK


#include "GameplayTags/CSGameplayTags.h"


namespace CSGameplayTags
{
	namespace Abilities
	{
		UE_DEFINE_GAMEPLAY_TAG(ActivateOnGiven, "CSGameplayTags.Abilities.ActivateOnGiven");

		UE_DEFINE_GAMEPLAY_TAG(Primary, "CSGameplayTags.Abilities.Primary");
		UE_DEFINE_GAMEPLAY_TAG(Secondary, "CSGameplayTags.Abilities.Secondary");
		UE_DEFINE_GAMEPLAY_TAG(Tertiary, "CSGameplayTags.Abilities.Tertiary");
		UE_DEFINE_GAMEPLAY_TAG(Roll, "CSGameplayTags.Abilities.Roll");
		UE_DEFINE_GAMEPLAY_TAG(Death, "CSGameplayTags.Abilities.Death");

	}

	namespace Events
	{
		UE_DEFINE_GAMEPLAY_TAG(HitReact, "CSGameplayTags.Events.HitReact");

	}

	namespace Status
	{
		UE_DEFINE_GAMEPLAY_TAG(Dead, "CSGameplayTags.Status.Dead");

	}
}