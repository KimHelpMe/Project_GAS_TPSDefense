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
	}

	namespace Events
	{
		UE_DEFINE_GAMEPLAY_TAG(HitReact, "CSGameplayTags.Events.HitReact");

	}
}