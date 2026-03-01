
#include "GASPrj/Public/Player/GP_PlayerState.h"

#include "AbilitySystemComponent.h"


AGP_PlayerState::AGP_PlayerState()
{
	SetNetUpdateFrequency(100.f);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

UAbilitySystemComponent* AGP_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
