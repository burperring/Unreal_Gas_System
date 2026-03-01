
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GP_PlayerState.generated.h"


class UAbilitySystemComponent;

UCLASS()
class GASPRJ_API AGP_PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGP_PlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "GP|Ability")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
