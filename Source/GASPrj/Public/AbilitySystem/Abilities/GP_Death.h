
#pragma once

#include "CoreMinimal.h"
#include "GP_GameplayAbility.h"
#include "GP_Death.generated.h"


class UAbilityTask_PlayMontageAndWait;

UCLASS()
class GASPRJ_API UGP_Death : public UGP_GameplayAbility
{
	GENERATED_BODY()

public:
	UGP_Death();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// UAbilityTask_PlayMontageAndWait Delegate callbacks
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageBlendOut();
	UFUNCTION()
	void OnMontageInterrupted();
	UFUNCTION()
	void OnMontageCancelled();
	// ===================================================
	
private:
	void PlayDeathMontage();
	void ApplyDeathEffect() const;
	void RespawnCharacter();
	void ResetDeathCharacter() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Montage", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> DeathMontage;
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
	UPROPERTY(EditDefaultsOnly, Category = "GP|Effects")
	TSubclassOf<UGameplayEffect> DeathEffect;		// GPTags::Status::Death
};
