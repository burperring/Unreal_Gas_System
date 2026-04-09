
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GP_GameplayAbility.h"
#include "GP_PlayerKillScored.generated.h"


class UAbilityTask_WaitGameplayEvent;

UCLASS()
class GASPRJ_API UGP_PlayerKillScored : public UGP_GameplayAbility
{
	GENERATED_BODY()

public:
	UGP_PlayerKillScored();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// UAbilityTask_WaitGameplayEvent Delegate Callback
	UFUNCTION()
	void OnEventReceived(FGameplayEventData Payload);
	// ===================================================

private:
	UFUNCTION()
	void WaitForGameplayEvent(FGameplayTag EventTag);

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitTask;
};
