
#include "Utils/GP_AbilitySystemBlueprintLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Character/GP_BaseCharacter.h"
#include "Character/GP_PlayerCharacter.h"
#include "Engine/OverlapResult.h"
#include "GameplayTags/GPTags.h"
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

void UGP_AbilitySystemBlueprintLibrary::SendPlayerHitReact(AActor* Target, FGameplayEventData& Payload, UObject* OptionalParticleSystem)
{
	AGP_PlayerCharacter* PC = Cast<AGP_PlayerCharacter>(Target);
	if (PC == nullptr) return;

	Payload.OptionalObject = OptionalParticleSystem;

	if (PC->IsAlive())
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PC, GPTags::Events::Player::HitReact, Payload);
	else
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PC, GPTags::GPAbilities::Death, Payload);
}

TArray<AActor*> UGP_AbilitySystemBlueprintLibrary::HitBoxOerlap(AActor* AvatarActor, float HitBoxRadius, float HitBoxForwardOffset, float HitBoxElevationOffset, bool bDrawDebugs)
{
	if (!IsValid(AvatarActor)) return TArray<AActor*>();

	// 아바타 액터에 대한 오버랩 감지 제거
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(AvatarActor);

	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);

	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(HitBoxRadius);

	const FVector Forward = AvatarActor->GetActorForwardVector() * HitBoxForwardOffset;
	const FVector HitBoxLocation = AvatarActor->GetActorLocation() + Forward + FVector(0.f, 0.f, HitBoxElevationOffset);

	UWorld* World = GEngine->GetWorldFromContextObject(AvatarActor, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World)) return TArray<AActor*>();
	
	World->OverlapMultiByChannel(OverlapResults, HitBoxLocation,
		FQuat::Identity, ECC_Visibility, Sphere, CollisionParams, ResponseParams);

	TArray<AActor*> OverlapActors;
	for (const FOverlapResult& Result : OverlapResults)
	{
		AGP_BaseCharacter* BaseCharacter = Cast<AGP_BaseCharacter>(Result.GetActor());
		if (BaseCharacter == nullptr) continue;
		if (!BaseCharacter->IsAlive()) continue;
		
		OverlapActors.AddUnique(BaseCharacter);
	}

	if (bDrawDebugs)
	{
		DrawHitBoxOverlapDebugs(AvatarActor, OverlapResults, HitBoxLocation, HitBoxRadius);
	}

	return OverlapActors;
}

void UGP_AbilitySystemBlueprintLibrary::DrawHitBoxOverlapDebugs(const UObject* WorldContextObject,
	const TArray<FOverlapResult>& OverlapResults, const FVector& HitBoxLocation, float HitBoxRadius)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World)) return;
	
	DrawDebugSphere(World, HitBoxLocation, HitBoxRadius, 16, FColor::White, false, 3.f);

	for (const FOverlapResult& Result : OverlapResults)
	{
		if (IsValid(Result.GetActor()))
		{
			FVector DebugLocation = Result.GetActor()->GetActorLocation();
			DebugLocation.Z += 100.f;
			DrawDebugSphere(World, DebugLocation, 30.f, 10, FColor::Green, false, 3.f);
		}
	}
}
