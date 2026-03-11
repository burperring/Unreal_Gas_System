
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Components/WidgetComponent.h"
#include "GP_WidgetComponent.generated.h"



class UGP_AttributeSet;
class UAbilitySystemComponent;
class UGP_AbilitySystemComponent;
class AGP_BaseCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GASPRJ_API UGP_WidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TMap<FGameplayAttribute, FGameplayAttribute> AttributeMap;

private:
	void InitAbilitySystemData();
	void InitializeAttributeDelegate();
	void BindWidgetToAttributeChanges(UWidget* WidgetObject, const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const;
	bool IsASCInitialized() const;

	UFUNCTION()
	void OnASCInitialized(UAbilitySystemComponent* ASC, UAttributeSet* AS);
	UFUNCTION()
	void BindToAttributeChanges();

	TWeakObjectPtr<AGP_BaseCharacter> BaseCharacter;
	TWeakObjectPtr<UGP_AbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<UGP_AttributeSet> AttributeSet;
};
