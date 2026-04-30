
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GP_GameplayAbility.h"
#include "GP_Primary.generated.h"


class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class GASPRJ_API UGP_Primary : public UGP_GameplayAbility
{
	GENERATED_BODY()

public:
	UGP_Primary();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// UAbilityTask_WaitGameplayEvent Delegate Callback
	virtual void OnEventReceived(FGameplayEventData Payload) override;
	// ===================================================
	
private:
	void ApplyBlockHitReactGE();
	void HitBoxOerlapApply();
	void SendHitReactEventToActors(const TArray<AActor*>& HitActors) const;
	void ApplyDamageEventToActors(const TArray<AActor*>& HitActors) const;
	void PlayMontageFlipFlop();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Montage", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> Montage_1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Montage", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> Montage_2;

	UPROPERTY(EditDefaultsOnly, Category = "GP|Effects")
	TSubclassOf<UGameplayEffect> PlayerDamageEffect;
	UPROPERTY(EditDefaultsOnly, Category = "GP|Effects")
	TSubclassOf<UGameplayEffect> BlockHitReactEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "GP|Abilities")
	float HitBoxRadius = 150.f;
	UPROPERTY(EditDefaultsOnly, Category = "GP|Abilities")
	float HitBoxForwardOffset = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "GP|Abilities")
	float HitBoxElevationOffset = 20.f;

	bool bFlip = true;
};
