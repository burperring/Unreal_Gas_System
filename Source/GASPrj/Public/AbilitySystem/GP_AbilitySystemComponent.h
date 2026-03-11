
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GP_AbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GASPRJ_API UGP_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UGP_AbilitySystemComponent();

	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRep_ActivateAbilities() override;

	UFUNCTION(BlueprintCallable, Category = "GP|Ability")
	void SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level);
	UFUNCTION(BlueprintCallable, Category = "GP|Ability")
	void AddAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Value = 1);

private:
	void HandleAutoActivateAbility(const FGameplayAbilitySpec& AbilitySpec);
};
