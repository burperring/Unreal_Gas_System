
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GP_AbilitySystemBlueprintLibrary.generated.h"


struct FGameplayEventData;
struct FGameplayEffectContextHandle;
struct FGameplayTag;
class UAbilitySystemComponent;
class UGameplayEffect;

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
	static FClosestActorWithTagResult FindClosestActorWithTag(UObject* WorldContextObject, const FVector& Origin, const FName& Tag, float SearchRange);

	UFUNCTION(BlueprintCallable)
	static FGameplayEffectContextHandle SendSetByCallerEvent(UAbilitySystemComponent*& ASC, const TSubclassOf<UGameplayEffect>& DamageEffect, FGameplayTag DataTag, float Damage);
	
	UFUNCTION(BlueprintCallable)
	static void SendPlayerHitReact(AActor* Target, UPARAM(ref) FGameplayEventData& Payload, UObject* OptionalParticleSystem = nullptr);

	static TArray<AActor*> HitBoxOerlap(AActor* AvatarActor, float HitBoxRadius, float HitBoxForwardOffset = 0.f, float HitBoxElevationOffset = 0.f, bool bDrawDebugs = false);

	static void DrawHitBoxOverlapDebugs(const UObject* WorldContextObject, const TArray<FOverlapResult>& OverlapResults, const FVector& HitBoxLocation, float HitBoxRadius);

	static void ApplyKnockBack(AActor* AvatarActor, const TArray<AActor*>& HitActors, float InnerRadius, float OuterRadius, float LaunchForceMagnitude, float RotationAngle = 45.f, bool bDrawDebugs = false);
};
