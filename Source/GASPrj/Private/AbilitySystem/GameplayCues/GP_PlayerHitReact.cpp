
#include "AbilitySystem/GameplayCues/GP_PlayerHitReact.h"

#include "AbilitySystemComponent.h"
#include "Character/GP_PlayerCharacter.h"
#include "GameplayTags/GPTags.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Utils/GP_AbilitySystemBlueprintLibrary.h"


AGP_PlayerHitReact::AGP_PlayerHitReact()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool AGP_PlayerHitReact::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (bDrawDebugs)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Player Hit");
	
	PlayHitReactMontage(MyTarget, Parameters);
	SpawnParticle(Parameters);
	
	UGameplayStatics::PlayWorldCameraShake(
		MyTarget,
		ImpactCameraShake,
		MyTarget->GetActorLocation(),
		1000.f,
		1000.f,
		1.f
	);
	
	return Super::OnExecute_Implementation(MyTarget, Parameters);
}

void AGP_PlayerHitReact::PlayHitReactMontage(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	AGP_PlayerCharacter* PC = Cast<AGP_PlayerCharacter>(MyTarget);
	if (PC == nullptr) return;

	UAbilitySystemComponent* ASC = PC->GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;

	if (ASC->HasMatchingGameplayTag(GPTags::GPAbilities::BlockHitReact)) return;

	UAnimInstance* AnimInstance = PC->GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(HitMontage);

	FVector ToInstigator = Parameters.GetInstigator()->GetActorLocation() - MyTarget->GetActorLocation();
	ToInstigator.Normalize();
	
	EHitDirection HitDirection{UGP_AbilitySystemBlueprintLibrary::GetHitDirection(MyTarget->GetActorForwardVector(), ToInstigator)};
	FName SectionName{UGP_AbilitySystemBlueprintLibrary::GetHitDirectionName(HitDirection)};
	AnimInstance->Montage_JumpToSection(SectionName);
}

void AGP_PlayerHitReact::SpawnParticle(const FGameplayCueParameters& Parameters)
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

