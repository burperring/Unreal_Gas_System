
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GP_GameplayAbility.h"
#include "GP_EnemyHitReact.generated.h"


class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class GASPRJ_API UGP_EnemyHitReact : public UGP_GameplayAbility
{
	GENERATED_BODY()

public:
	virtual void BeginDestroy() override;
	
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
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Montage", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> HitMontage;

	void CacheHitDirectionVectors(const AActor* Instigator);

	FVector AvatarForwardVector;
	FVector ToInstigator;
};
