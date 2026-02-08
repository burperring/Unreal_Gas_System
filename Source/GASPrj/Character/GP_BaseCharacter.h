
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GP_BaseCharacter.generated.h"

UCLASS(Abstract)
class GASPRJ_API AGP_BaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGP_BaseCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};
