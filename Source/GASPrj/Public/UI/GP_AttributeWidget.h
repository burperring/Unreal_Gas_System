// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystem/GP_AttributeSet.h"
#include "GP_AttributeWidget.generated.h"


class UProgressBar;

UCLASS()
class GASPRJ_API UGP_AttributeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void OnAttributeChange(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair, UGP_AttributeSet* AttributeSet);
	void OnAttributeChange(float NewValue, float NewMaxValue);
	bool MatchesAttributes(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Attributes")
	FGameplayAttribute Attribute;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Attributes")
	FGameplayAttribute MaxAttribute;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* CPP_ProgressBar;
};
