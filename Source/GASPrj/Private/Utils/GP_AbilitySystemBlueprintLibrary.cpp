
#include "Utils/GP_AbilitySystemBlueprintLibrary.h"

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
