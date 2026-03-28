
#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GP_PlayerHitReact.generated.h"


class UCameraShakeBase;

UCLASS()
class GASPRJ_API AGP_PlayerHitReact : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	AGP_PlayerHitReact();

	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

private:
	void PlayHitReactMontage(AActor* MyTarget, const FGameplayCueParameters& Parameters);
	void SpawnParticle(const FGameplayCueParameters& Parameters);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Montage", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> HitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Camera", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShakeBase> ImpactCameraShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GP|Ability", meta = (AllowPrivateAccess = "true"))
	bool bDrawDebugs = false;
};
