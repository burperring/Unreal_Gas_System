
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GP_PlayerController.generated.h"


class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class GASPRJ_API AGP_PlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void SetupInputComponent() override;

private:
	void Jump();
	void StopJumping();
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void Primary();
	
	UPROPERTY(EditDefaultsOnly, Category = "GP|Input")
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;
	
	UPROPERTY(EditDefaultsOnly, Category = "GP|Input|Movement")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, Category = "GP|Input|Movement")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, Category = "GP|Input|Movement")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, Category = "GP|Input|Ability")
	TObjectPtr<UInputAction> PrimaryAction;
};
