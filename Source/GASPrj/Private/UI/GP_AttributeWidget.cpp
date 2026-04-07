
#include "UI/GP_AttributeWidget.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetStringLibrary.h"


void UGP_AttributeWidget::OnAttributeChange(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair, UGP_AttributeSet* AttributeSet, float OldValue)
{
	const float AttributeValue = Pair.Key.GetNumericValue(AttributeSet);
	const float MaxAttributeValue = Pair.Value.GetNumericValue(AttributeSet);

	OnAttributeChange(AttributeValue, MaxAttributeValue, OldValue);
}

void UGP_AttributeWidget::OnAttributeChange(float NewValue, float NewMaxValue, float OldValue)
{
	CPP_ProgressBar->SetPercent(NewValue / NewMaxValue);

	if (NewValue <= 0)
		SetVisibility(ESlateVisibility::Collapsed);
	else
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	SpawnDamagedParticle(NewValue, OldValue);
}

void UGP_AttributeWidget::SpawnDamagedParticle(float NewValue, float OldValue)
{
	if (!bIsShowNumberParticle) return;
	if (OldValue == 0.f) return;

	int32 iValueChange = floor(abs(NewValue - OldValue));
	FString ValueChangeString = FString::FromInt(iValueChange);

	float fTextOffset = 0.f;
	TArray<FString> StringArr = UKismetStringLibrary::GetCharacterArrayFromString(ValueChangeString);
	for (FString& String : StringArr)
	{
		FVector AvatarActorLocation = AvatarActor->GetActorLocation();
		FVector ParticleSpawnLocation = AvatarActorLocation + FVector(0.f, 0.f, VerticalOffset);

		UNiagaraComponent* NumberSystem = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),                // World context
			NiagaraParticle,           // The Niagara System asset
			ParticleSpawnLocation,     // Spawn location
			FRotator::ZeroRotator,     // Spawn rotation
			FVector(1.f),           // Scale
			true                       // Auto-destroy when finished
		);

		FRotator CameraRotator = GetOwningPlayer()->PlayerCameraManager->GetCameraRotation();
		FVector CameraRightVector = CameraRotator.Quaternion().GetRightVector() * fTextOffset;
		
		int32 Index = FCString::Atoi(*String);
		if (NumberTextures.IsValidIndex(Index) && NumberTextures[Index])
		{
			FName TargetName = FName(TEXT("User.Digit"));
			NumberSystem->SetVariableTexture(TargetName, NumberTextures[Index]);
		}
		NumberSystem->AddLocalOffset(CameraRightVector);

		fTextOffset += NumberSpacing;
	}
}

bool UGP_AttributeWidget::MatchesAttributes(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const
{
	return Attribute == Pair.Key && MaxAttribute == Pair.Value;
}
