// CSK


#include "AbilitySystem/CSAbilitySystemComponent.h"


UCSAbilitySystemComponent::UCSAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCSAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCSAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}
