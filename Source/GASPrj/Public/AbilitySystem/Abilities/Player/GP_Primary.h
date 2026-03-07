
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
	virtual void BeginDestroy() override;
	
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
	void HitBoxOerlapTest();
	void DrawHitBoxOverlapDebugs(const TArray<FOverlapResult>& OverlapResults, const FVector& HitBoxLocation) const;
	void SendHitReactEventToActors(const TArray<AActor*>& HitActors) const;
	void PlayMontageFlipFlop();

	UFUNCTION()
	void WaitForGameplayEvent(FGameplayTag EventTag);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Montage_1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Montage_2;
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* MontageTask;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitTask;
	
	UPROPERTY(EditDefaultsOnly, Category = "GP|Abilities")
	float HitBoxRadius = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "GP|Abilities")
	float HitBoxForwardOffset = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "GP|Abilities")
	float HitBoxElevationOffset = 20.f;

	bool bFlip = true;
};
