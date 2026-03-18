
#include "AbilitySystem/Abilities/Player/GP_PlayerKillScored.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameplayTags/GPTags.h"


UGP_PlayerKillScored::UGP_PlayerKillScored()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UGP_PlayerKillScored::BeginDestroy()
{
	Super::BeginDestroy();

	if (WaitTask != nullptr)
	{
		WaitTask->EventReceived.RemoveAll(this);
	}
}

void UGP_PlayerKillScored::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	WaitForGameplayEvent(GPTags::Events::KillScored);
}

void UGP_PlayerKillScored::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
									const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGP_PlayerKillScored::OnEventReceived(FGameplayEventData Payload)
{
	if (bDrawDebugs)
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, FString::Printf(TEXT("KillScored Event: %s"), *Payload.Instigator.GetName()));

	
}

void UGP_PlayerKillScored::WaitForGameplayEvent(FGameplayTag EventTag)
{
	WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, 
		EventTag, 
		/* OptionalExternalTarget (ASC to listen on) */ nullptr, 
		/* OnlyTriggerOnce */ false, 
		/* OnlyMatchExact */ true
	);

	if (WaitTask)
	{
		WaitTask->EventReceived.AddDynamic(this, &UGP_PlayerKillScored::OnEventReceived);
		WaitTask->ReadyForActivation();
	}
}
