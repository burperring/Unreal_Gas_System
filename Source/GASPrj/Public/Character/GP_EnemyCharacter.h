// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GP_BaseCharacter.h"
#include "GP_EnemyCharacter.generated.h"


class UAttributeSet;
class UGP_AbilitySystemComponent;

UCLASS()
class GASPRJ_API AGP_EnemyCharacter : public AGP_BaseCharacter
{
	GENERATED_BODY()

public:
	AGP_EnemyCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const override;
	virtual void HandleRespawn();

	void RotateToTarget(AActor* Target);
	void StopMovementUntilLanded();
	
	FORCEINLINE float GetEnemyAcceptanceRadius() const { return AcceptanceRadius; }
	FORCEINLINE float GetEnemyMinAttackDelay() const { return MinAttackDelay; }
	FORCEINLINE float GetEnemyMaxAttackDelay() const { return MaxAttackDelay; }
	FORCEINLINE float GetTimerLength() const { return TimerLength; }
	FORCEINLINE bool GetEnemyIsLaunched() const { return bIsBeingLaunched; }

protected:
	virtual void BeginPlay() override;
	virtual void HandleDeath() override;

private:
	void UpdateRotationToTarget();
	void StopTimer();

	UFUNCTION()
	void EnableMovementOnLanded(const FHitResult& Hit);
	
	UPROPERTY(VisibleAnywhere, Category = "GP|AbilitySystem")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|AI", meta = (AllowPrivateAccess = "true"))
	float AcceptanceRadius{500.f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|AI", meta = (AllowPrivateAccess = "true"))
	float MinAttackDelay{0.1f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|AI", meta = (AllowPrivateAccess = "true"))
	float MaxAttackDelay{0.5f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|AI", meta = (AllowPrivateAccess = "true"))
	float TimerLength{0.2f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	bool bIsBeingLaunched = false;
	
	// 회전을 제어할 타이머 핸들
	FTimerHandle RotationTimerHandle;
	FTimerHandle StopTimerHandle;
	UPROPERTY()
	AActor* CurrentTarget;
};
