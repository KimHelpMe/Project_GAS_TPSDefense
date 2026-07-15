


#include "Data/CSCharacterAbilityData.h"

UAnimMontage* UCSCharacterAbilityData::GetMontage(FGameplayTag AbilityTag) const
{
    if (const TObjectPtr<UAnimMontage>* Found = MontageMap.Find(AbilityTag))
    {
        return *Found;
    }

    return nullptr;
}
