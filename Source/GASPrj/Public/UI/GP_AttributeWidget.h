// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystem/GP_AttributeSet.h"
#include "GP_AttributeWidget.generated.h"


class UProgressBar;
class UNiagaraSystem;
class UTexture;

UCLASS()
class GASPRJ_API UGP_AttributeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void OnAttributeChange(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair, UGP_AttributeSet* AttributeSet, float OldValue);
	void OnAttributeChange(float NewValue, float NewMaxValue, float OldValue);
	void SpawnDamagedParticle(float NewValue, float OldValue);
	bool MatchesAttributes(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const;

	FORCEINLINE void SetAvatarActor(const TObjectPtr<AActor>& Actor) { AvatarActor = Actor; }
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Attributes")
	FGameplayAttribute Attribute;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Attributes")
	FGameplayAttribute MaxAttribute;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* CPP_ProgressBar;

private:
	UPROPERTY(BlueprintReadOnly, Category = "GP|Attributes", meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AActor> AvatarActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|FX", meta = (AllowPrivateAccess = "true"))
	bool bIsShowNumberParticle{false};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|FX", meta = (AllowPrivateAccess = "true"))
	float VerticalOffset{200.f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|FX", meta = (AllowPrivateAccess = "true"))
	float NumberSpacing{10.f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|FX", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* NiagaraParticle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|FX", meta = (AllowPrivateAccess = "true"))
	TArray<UTexture*> NumberTextures;
};
