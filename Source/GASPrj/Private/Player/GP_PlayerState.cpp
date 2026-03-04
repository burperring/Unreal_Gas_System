
#include "GASPrj/Public/Player/GP_PlayerState.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/GP_AbilitySystemComponent.h"


AGP_PlayerState::AGP_PlayerState()
{
	SetNetUpdateFrequency(100.f);

	AbilitySystemComponent = CreateDefaultSubobject<UGP_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

UAbilitySystemComponent* AGP_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
