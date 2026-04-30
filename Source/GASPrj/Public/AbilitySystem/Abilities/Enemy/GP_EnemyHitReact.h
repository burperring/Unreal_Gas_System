
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
	UGP_EnemyHitReact();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// UAbilityTask_WaitGameplayEvent Delegate Callback
	virtual void OnEventReceived(FGameplayEventData Payload) override;
	// ===================================================
	
private:
	void CacheHitDirectionVectors(const AActor* Instigator);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Montage", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> HitMontage;

	FVector AvatarForwardVector;
	FVector ToInstigator;
};
