
#include "AbilitySystem/Abilities/Player/GP_Secondary.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/GP_BaseCharacter.h"
#include "GameplayTags/GPTags.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/GP_AbilitySystemBlueprintLibrary.h"


UGP_Secondary::UGP_Secondary()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGP_Secondary::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// SecondarySpecHandle = Handle;
	// SecondaryActorInfo = *ActorInfo;
	// SecondaryActivationInfo = ActivationInfo;
	
	// Cost Gameplay Effect Class에 세팅한 GE를 발동시킨다.
	// CommitAbilityCost(Handle, ActorInfo, ActivationInfo);
	// Cooldown Gameplay Effect Class에 세팅한 GE를 발동시킨다.
	// CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Cost와 Cooldown GE를 동시에 발동시켜준다.
	CommitAbility(Handle, ActorInfo, ActivationInfo);
	
	ApplyBlockHitReactGE();
	PlayGuardMontage();
	WaitForGameplayEvent(GPTags::Events::Player::Secondary);
}

void UGP_Secondary::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(GPTags::GPAbilities::BlockHitReact);
	BP_RemoveGameplayEffectFromOwnerWithGrantedTags(TagContainer, 1);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGP_Secondary::ApplyBlockHitReactGE()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!IsValid(ASC)) return;

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(BlockHitReactEffect, GetAbilityLevel(), ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UGP_Secondary::PlayGuardMontage()
{
	if (GuardMontage == nullptr)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	PlayMontageAndWait(GuardMontage);
}

void UGP_Secondary::OnEventReceived(FGameplayEventData Payload)
{
	// Received에 집어넣게 되면 현재 세팅상 애님몽타주 실행 도중에 Cost가 발생한다.
	// 이렇게 중간에 넣게 되더라도 Cost에 사용할 비용이 없을 경우 Ability를 실행하지 못하게 막는다.
	// CommitAbilityCost(SecondarySpecHandle, &SecondaryActorInfo, SecondaryActivationInfo);
	
	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		AbilityParticle,									// UParticleSystem*
		GetAvatarActorFromActorInfo()->GetActorLocation(),	// FVector
		FRotator::ZeroRotator,								// FRotator
		true												// AutoDestroy
	);

	UGameplayStatics::PlayWorldCameraShake(
		GetAvatarActorFromActorInfo(),
		ImpactCameraShake,
		GetAvatarActorFromActorInfo()->GetActorLocation(),
		1000.f,
		1000.f,
		1.f
	);

	HitBoxOerlapApply();
}

void UGP_Secondary::HitBoxOerlapApply()
{
	TArray<AActor*> OverlapActors{};
	OverlapActors = UGP_AbilitySystemBlueprintLibrary::HitBoxOerlap(
		GetAvatarActorFromActorInfo(),
		HitBoxRadius,
		0.f,
		0.f,
		bDrawDebugs
	);

	UGP_AbilitySystemBlueprintLibrary::ApplyKnockBack(
		GetAvatarActorFromActorInfo(),
		OverlapActors,
		InnerRadius,
		HitBoxRadius,
		LaunchForceMagnitude,
		RotationAngle,
		bDrawDebugs
	);

	for (AActor* OverlapActor : OverlapActors)
	{
		AGP_BaseCharacter* BaseCharacter = Cast<AGP_BaseCharacter>(OverlapActor);
		if (BaseCharacter == nullptr) continue;

		UAbilitySystemComponent* ASC = BaseCharacter->GetAbilitySystemComponent();
		if (!IsValid(ASC)) continue;

		// Send Set By Caller Damage Event
		UGP_AbilitySystemBlueprintLibrary::SendSetByCallerEvent(ASC, DamageEffect, GPTags::SetByCaller::Player::Secondary, Damage);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
