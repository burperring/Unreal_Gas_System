
#pragma once

#include "CoreMinimal.h"
#include "GP_EnemyAttack.h"
#include "GP_EnemyMeleeAttack.generated.h"


class UAbilityTask_WaitGameplayEvent;

UCLASS()
class GASPRJ_API UGP_EnemyMeleeAttack : public UGP_EnemyAttack
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// UAbilityTask_WaitGameplayEvent Delegate Callback
	virtual void OnEventReceived(FGameplayEventData Payload) override;
	// ===================================================

private:
	bool IsEnemyDeath();

	UPROPERTY(EditDefaultsOnly, Category = "GP|Damage")
	TSubclassOf<UGameplayEffect> DamageEffect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Damage", meta = (ExposeOnSpawn, ClampMax = "0.0", AllowPrivateAccess = true))
	float Damage{-10.f};
	UPROPERTY(EditDefaultsOnly, Category = "GP|Particle")
	UParticleSystem* ImpactParticles;
};
