
#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GP_GameplayAbility.generated.h"


class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class GASPRJ_API UGP_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGP_GameplayAbility();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// UAbilityTask_PlayMontageAndWait Delegate callbacks
	UFUNCTION()
	virtual void OnMontageCompleted();
	UFUNCTION()
	virtual void OnMontageBlendIn();
	UFUNCTION()
	virtual void OnMontageBlendOut();
	UFUNCTION()
	virtual void OnMontageInterrupted();
	UFUNCTION()
	virtual void OnMontageCancelled();
	// ===================================================

	// UAbilityTask_WaitGameplayEvent Delegate Callback
	UFUNCTION()
	virtual void OnEventReceived(FGameplayEventData Payload);
	// ===================================================

	// GAS Event system setting
	void PlayMontageAndWait(TObjectPtr<UAnimMontage> Montage, float Rate = 1.f, FName StartSection = NAME_None, bool bStopWhenAbilityEnds = true, float AnimRootMotionTranslationScale = 1.f);
	void WaitForGameplayEvent(FGameplayTag EventTag, AActor* OptionalExternalTarget = nullptr, bool bOnlyTriggerOnce = false, bool bOnlyMatchExact = true);
	// ========================
	
private:
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitTask;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GP|Ability")
	bool bDrawDebugs = false;
};
