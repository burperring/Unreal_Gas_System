
#include "AbilitySystem/Abilities/Enemy/GP_EnemyMeleeAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/GP_PlayerCharacter.h"
#include "GameplayTags/GPTags.h"
#include "Particles/ParticleSystem.h"
#include "Utils/GP_AbilitySystemBlueprintLibrary.h"



void UGP_EnemyMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (bDrawDebugs)
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("EnemyMeleeAttack Active!"));
	
	if (IsEnemyDeath()) return;

	WaitForGameplayEvent(GPTags::Events::Enemy::MeleeTraceHit);
}

void UGP_EnemyMeleeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGP_EnemyMeleeAttack::OnEventReceived(FGameplayEventData Payload)
{
	if (IsEnemyDeath()) return;
	
	const FGameplayEffectContextHandle ContextHandle = Payload.ContextHandle;
	const FHitResult* HitResult = ContextHandle.GetHitResult();
	
	if (bDrawDebugs)
	{
		DrawDebugSphere(GetWorld(), HitResult->ImpactPoint, 25, 16, FColor::Green, false, 5.f);
	}

	AGP_PlayerCharacter* PC = Cast<AGP_PlayerCharacter>(HitResult->GetActor());
	if (PC == nullptr) return;
	if (!PC->IsAlive()) return;

	UAbilitySystemComponent* ASC = PC->GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;

	// Send Set By Caller Damage Event
	UGP_AbilitySystemBlueprintLibrary::SendSetByCallerEvent(ASC, DamageEffect, GPTags::SetByCaller::MeleeTraceHit, Damage);

	// Send Player Hit React
	UGP_AbilitySystemBlueprintLibrary::SendPlayerHitReact(PC, Payload, ImpactParticles);
}

bool UGP_EnemyMeleeAttack::IsEnemyDeath()
{
	// Enemuy 사망 여부 체크
	if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(GPTags::Status::Death))
	{
		FGameplayEventData Payload;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActorFromActorInfo(), GPTags::Events::Enemy::EndAttack, Payload);
		return true;
	}
	
	return false;
}