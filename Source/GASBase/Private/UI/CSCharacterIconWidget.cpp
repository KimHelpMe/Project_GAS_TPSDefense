// CSK

#include "UI/CSCharacterIconWidget.h"
#include "Data/CSCharacterSelectData.h"

UTexture2D* UCSCharacterIconWidget::GetIconTexture(TSubclassOf<APawn> InCharacterClass) const
{
	if (!CharacterSelectData)
	{
		return nullptr;
	}

	const ECSCharacterID ID = CharacterSelectData->FindIDByClass(InCharacterClass);
	const FCSCharacterSelectEntry* Entry = CharacterSelectData->FindEntryByID(ID);
	return Entry ? Entry->IconTexture : nullptr;
}
