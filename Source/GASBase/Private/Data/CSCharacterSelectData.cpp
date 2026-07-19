// CSK

#include "Data/CSCharacterSelectData.h"

const FCSCharacterSelectEntry* UCSCharacterSelectData::FindEntryByID(ECSCharacterID InID) const
{
	return Entries.Find(InID);
}

ECSCharacterID UCSCharacterSelectData::FindIDByClass(TSubclassOf<APawn> InClass) const
{
	for (const auto& Pair : Entries)
	{
		if (Pair.Value.CharacterClass == InClass)
		{
			return Pair.Key;
		}
	}
	return ECSCharacterID::None;
}
