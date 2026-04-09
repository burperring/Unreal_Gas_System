
#include "AbilitySystem/Abilities/Enemy/GP_EnemyHitReact.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/Abilities/Player/GP_Primary.h"
#include "Character/GP_BaseCharacter.h"
#include "GameplayTags/GPTags.h"
#include "Utils/GP_AbilitySystemBlueprintLibrary.h"

UGP_EnemyHitReact::UGP_EnemyHitReact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UGP_EnemyHitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (bDrawDebugs)
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Activate HitReact!"));
	
	WaitForGameplayEvent(GPTags::Events::Enemy::HitReact);
}

void UGP_EnemyHitReact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (WaitTask != nullptr) WaitTask->EndTask();
}

void UGP_EnemyHitReact::OnEventReceived(FGameplayEventData Payload)
{
	if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(GPTags::Status::Death)) return;
	
	CacheHitDirectionVectors(Payload.Instigator);

	EHitDirection HitDirection{UGP_AbilitySystemBlueprintLibrary::GetHitDirection(AvatarForwardVector, ToInstigator)};
	FName SectionName{UGP_AbilitySystemBlueprintLibrary::GetHitDirectionName(HitDirection)};

	if (bDrawDebugs)
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Hit React Event : %s"), *SectionName.ToString()));

	if (HitMontage == nullptr)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName(""),	// Task Instance Name (can be empty)
		HitMontage,
		1.f,
		SectionName,	// Start Section
		true,	// bStopWhenAbilityEnds
		1.0f	// AnimRootMotionTranslationScale
	);

	if (MontageTask)
	{
		MontageTask->ReadyForActivation();
	}
}

void UGP_EnemyHitReact::WaitForGameplayEvent(FGameplayTag EventTag)
{
	WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, 
		EventTag, 
		/* OptionalExternalTarget (ASC to listen on) */ nullptr, 
		/* OnlyTriggerOnce */ false, 
		/* OnlyMatchExact */ true
	);

	if (WaitTask)
	{
		WaitTask->EventReceived.AddDynamic(this, &UGP_EnemyHitReact::OnEventReceived);
		WaitTask->ReadyForActivation();
	}
}

void UGP_EnemyHitReact::CacheHitDirectionVectors(const AActor* Instigator)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor == nullptr) return;
	
	AvatarForwardVector = AvatarActor->GetActorForwardVector();

	const FVector AvatarLocation = AvatarActor->GetActorLocation();
	const FVector InstigatorLocation = Instigator->GetActorLocation();

	ToInstigator = InstigatorLocation - AvatarLocation;
	ToInstigator.Normalize();
}
