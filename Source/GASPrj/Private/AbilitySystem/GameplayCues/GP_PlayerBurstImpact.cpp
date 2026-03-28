
#include "AbilitySystem/GameplayCues/GP_PlayerBurstImpact.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"


UGP_PlayerBurstImpact::UGP_PlayerBurstImpact()
{
}

bool UGP_PlayerBurstImpact::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	SpawnParticle(Parameters);
	
	return Super::OnExecute_Implementation(Target, Parameters);
}

void UGP_PlayerBurstImpact::SpawnParticle(const FGameplayCueParameters& Parameters) const
{
	UParticleSystem* ParticleSystem = Cast<UParticleSystem>(const_cast<UObject*>(Parameters.GetSourceObject()));
	if (ParticleSystem == nullptr) return;

	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		ParticleSystem,											// UParticleSystem*
		Parameters.EffectContext.GetHitResult()->ImpactPoint,	// FVector
		FRotator::ZeroRotator,									// FRotator
		true													// AutoDestroy
	);
}
