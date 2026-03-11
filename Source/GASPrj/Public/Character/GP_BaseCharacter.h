
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GP_BaseCharacter.generated.h"


class UAttributeSet;
class UGameplayAbility;
class UGameplayEffect;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FASCInitialized, UAbilitySystemComponent*, ASC, UAttributeSet*, AS);

UCLASS(Abstract)
class GASPRJ_API AGP_BaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGP_BaseCharacter();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const { return nullptr; }

	// Ability System Component가 제대로 생성되었는지 확인하기 위한 델리게이트
	UPROPERTY(BlueprintAssignable)
	FASCInitialized OnASCInitialized;
	
protected:
	void GiveStartupAbilities();
	void InitializeAttributes();
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "GP|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	UPROPERTY(EditDefaultsOnly, Category = "GP|Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;
};
