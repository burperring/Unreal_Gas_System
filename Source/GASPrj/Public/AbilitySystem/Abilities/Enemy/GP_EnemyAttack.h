
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GP_GameplayAbility.h"
#include "GP_EnemyAttack.generated.h"


class UAbilityTask_PlayMontageAndWait;

UCLASS()
class GASPRJ_API UGP_EnemyAttack : public UGP_GameplayAbility
{
	GENERATED_BODY()

public:
	UGP_EnemyAttack();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// UAbilityTask_PlayMontageAndWait Delegate callbacks
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageInterrupted();
	UFUNCTION()
	void OnMontageCancelled();
	// ===================================================

private:
	void PlayEnemyAttackMontage();
	void SpawnProjectile();
	void SendEndAttackEventTag();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GP|Projectile", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> EnemyProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Montage", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> AttackMontageArray;
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GP|Projectile", meta = (AllowPrivateAccess = "true"))
	FName MuzzleSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Type", meta = (AllowPrivateAccess = "true"))
	bool bIsMelee = false;
};
