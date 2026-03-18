
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GP_AbilitySystemBlueprintLibrary.generated.h"


UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	EHD_Left UMETA(DisplayName = "Left"),
	EHD_Right UMETA(DisplayName = "Right"),
	EHD_Front UMETA(DisplayName = "Front"),
	EHD_Back UMETA(DisplayName = "Back"),

	EHD_Max UMETA(DisplayName = "Max")
};

USTRUCT(BlueprintType)
struct FClosestActorWithTagResult
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> Actor;

	UPROPERTY(BlueprintReadWrite)
	float Distance{0.f};
};

UCLASS()
class GASPRJ_API UGP_AbilitySystemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static EHitDirection GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator);

	UFUNCTION(BlueprintPure)
	static FName GetHitDirectionName(const EHitDirection& HitDirection);

	UFUNCTION(BlueprintCallable)
	static FClosestActorWithTagResult FindClosestActorWithTag(const UObject* WorldContextObject, const FVector& Origin, const FName& Tag);
};
