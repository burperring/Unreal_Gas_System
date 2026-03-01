
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GP_PlayerController.generated.h"


struct FGameplayTag;
struct FInputActionValue;
class UInputMappingContext;
class UInputAction;

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
	void Secondary();
	void Tertiary();
	void ActivateAbility(const FGameplayTag& AbilityTag) const;
	
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
	UPROPERTY(EditDefaultsOnly, Category = "GP|Input|Ability")
	TObjectPtr<UInputAction> SecondaryAction;
	UPROPERTY(EditDefaultsOnly, Category = "GP|Input|Ability")
	TObjectPtr<UInputAction> TertiaryAction;
};
