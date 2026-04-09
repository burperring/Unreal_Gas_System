
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GP_GameplayAbility.h"
#include "GP_Secondary.generated.h"


class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class GASPRJ_API UGP_Secondary : public UGP_GameplayAbility
{
	GENERATED_BODY()

public:
	UGP_Secondary();
	
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

	// UAbilityTask_WaitGameplayEvent Delegate Callback
	UFUNCTION()
	void OnEventReceived(FGameplayEventData Payload);
	// ===================================================

private:
	void ApplyBlockHitReactGE();
	void PlayGuardMontage();
	void HitBoxOerlapApply();

	UFUNCTION()
	void WaitForGameplayEvent(FGameplayTag EventTag);

	UPROPERTY(EditDefaultsOnly, Category = "GP|Damage")
	TSubclassOf<UGameplayEffect> DamageEffect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Damage", meta = (ExposeOnSpawn, ClampMax = "0.0", AllowPrivateAccess = true))
	float Damage{-40.f};
	
	UPROPERTY(EditDefaultsOnly, Category = "GP|Effects")
	TSubclassOf<UGameplayEffect> BlockHitReactEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Montage", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> GuardMontage;
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitTask;

	UPROPERTY(EditDefaultsOnly, Category = "GP|Particle")
	UParticleSystem* AbilityParticle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Camera", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShakeBase> ImpactCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "GP|Abilities|HitBox")
	float HitBoxRadius = 1500.f;
	UPROPERTY(EditDefaultsOnly, Category = "GP|Abilities|KnockBack")
	float InnerRadius = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = "GP|Abilities|KnockBack")
	float LaunchForceMagnitude = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "GP|Abilities|KnockBack")
	float RotationAngle = 65.f;

	//FGameplayAbilitySpecHandle SecondarySpecHandle{};
	//FGameplayAbilityActorInfo SecondaryActorInfo{};
	//FGameplayAbilityActivationInfo SecondaryActivationInfo{};
};
