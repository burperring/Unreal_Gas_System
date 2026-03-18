
#include "Utils/GP_AbilitySystemBlueprintLibrary.h"

#include "Character/GP_BaseCharacter.h"
#include "Kismet/GameplayStatics.h"

EHitDirection UGP_AbilitySystemBlueprintLibrary::GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator)
{
	// Target과 Instigator의 cos 값을 통해 어디서 맞았는지 체크
	const float Dot = FVector::DotProduct(TargetForward, ToInstigator);

	if (Dot < -0.5f)
	{
		return EHitDirection::EHD_Back;
	}
	if (Dot < 0.5f) // Left or Right
	{
		const FVector Cross = FVector::CrossProduct(TargetForward, ToInstigator);
		if (Cross.Z < 0.f)
			return EHitDirection::EHD_Left;
		
		return EHitDirection::EHD_Right;
	}
	return EHitDirection::EHD_Front;
}

FName UGP_AbilitySystemBlueprintLibrary::GetHitDirectionName(const EHitDirection& HitDirection)
{
	switch (HitDirection)
	{
	case EHitDirection::EHD_Left: return FName("Left");
	case EHitDirection::EHD_Right: return FName("Right");
	case EHitDirection::EHD_Front: return FName("Front");
	case EHitDirection::EHD_Back: return FName("Back");
	default: return FName("None");
	}
}

FClosestActorWithTagResult UGP_AbilitySystemBlueprintLibrary::FindClosestActorWithTag(const UObject* WorldContextObject,
	const FVector& Origin, const FName& Tag)
{
	TArray<AActor*> ActorsWithTag;
	UGameplayStatics::GetAllActorsWithTag(WorldContextObject, Tag, ActorsWithTag);

	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;

	for (AActor* Actor : ActorsWithTag)
	{
		if (!IsValid(Actor)) continue;

		AGP_BaseCharacter* BaseCharacter = Cast<AGP_BaseCharacter>(Actor);
		if (BaseCharacter == nullptr) continue;
		if (!BaseCharacter->IsAlive()) continue;

		const float Distance = FVector::Dist(Origin, Actor->GetActorLocation());
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestActor = Actor;
		}
	}

	FClosestActorWithTagResult Result;
	Result.Actor = ClosestActor;
	Result.Distance = ClosestDistance;

	return Result;
}
