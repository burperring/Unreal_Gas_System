
#pragma once

#include "CoreMinimal.h"
#include "GP_BaseCharacter.h"
#include "GP_PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class GASPRJ_API AGP_PlayerCharacter : public AGP_BaseCharacter
{
	GENERATED_BODY()

public:
	AGP_PlayerCharacter();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
};
