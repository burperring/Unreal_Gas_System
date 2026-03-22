
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GP_GameplayAbility.h"
#include "GP_PlayerImpactCues.generated.h"


class UAbilityTask_WaitGameplayEvent;

UCLASS()
class GASPRJ_API UGP_PlayerImpactCues : public UGP_GameplayAbility
{
	GENERATED_BODY()

public:
	UGP_PlayerImpactCues();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// UAbilityTask_WaitGameplayEvent Delegate Callback
	UFUNCTION()
	void OnHitReactEventReceived(FGameplayEventData Payload);
	UFUNCTION()
	void OnDeathEventReceived(FGameplayEventData Payload);
	// ===================================================

private:
	void WaitForGameplayEvent(TObjectPtr<UAbilityTask_WaitGameplayEvent>& WaitTask, FGameplayTag EventTag,
		void (UGP_PlayerImpactCues::*CallbackFunc)(FGameplayEventData));
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitHitReactTask;
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitDeathTask;
};
