// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GP_BaseCharacter.h"
#include "GP_EnemyCharacter.generated.h"

class UGP_AbilitySystemComponent;

UCLASS()
class GASPRJ_API AGP_EnemyCharacter : public AGP_BaseCharacter
{
	GENERATED_BODY()

public:
	AGP_EnemyCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "GP|AbilitySystem")
	TObjectPtr<UGP_AbilitySystemComponent> AbilitySystemComponent;
};
