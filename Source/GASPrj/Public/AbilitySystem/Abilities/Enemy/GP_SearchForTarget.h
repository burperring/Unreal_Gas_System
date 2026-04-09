
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GP_GameplayAbility.h"
#include "GP_SearchForTarget.generated.h"


namespace EPathFollowingResult
{
	enum Type : int;
}

class AGP_BaseCharacter;
class AAIController;
class AGP_EnemyCharacter;
class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_WaitDelay;
class UAITask_MoveTo;

UCLASS()
class GASPRJ_API UGP_SearchForTarget : public UGP_GameplayAbility
{
	GENERATED_BODY()

public:
	UGP_SearchForTarget();

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
	void StartSearch();
	void MoveTargetAndAttack();
	
	UFUNCTION()
	void Search();
	UFUNCTION()
	void Attack();
	UFUNCTION()
	void WaitForGameplayEvent(FGameplayTag EventTag);
	UFUNCTION()
	void AttackTarget(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController);
	
	TWeakObjectPtr<AGP_EnemyCharacter> OwningEnemy;
	TWeakObjectPtr<AAIController> OwningAIController;
	TWeakObjectPtr<AGP_BaseCharacter> TargetBaseCharacter;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitTask;
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> SearchDelayTask;
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> AttackDelayTask;
	UPROPERTY()
	TObjectPtr<UAITask_MoveTo> MoveToLocationOrActorTask;
};
