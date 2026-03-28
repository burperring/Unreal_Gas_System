
#include "AbilitySystem/Abilities/Player/GP_Secondary.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameplayTags/GPTags.h"
#include "Kismet/GameplayStatics.h"


UGP_Secondary::UGP_Secondary()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGP_Secondary::BeginDestroy()
{
	Super::BeginDestroy();

	if (MontageTask != nullptr)
	{
		MontageTask->OnCompleted.RemoveAll(this);
		MontageTask->OnBlendOut.RemoveAll(this);
		MontageTask->OnInterrupted.RemoveAll(this);
		MontageTask->OnCancelled.RemoveAll(this);
	}
	if (WaitTask != nullptr)
	{
		WaitTask->EventReceived.RemoveAll(this);
	}
}

void UGP_Secondary::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ApplyBlockHitReactGE();
	PlayGuardMontage();
	WaitForGameplayEvent(GPTags::Events::Player::Secondary);
}

void UGP_Secondary::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(GPTags::GPAbilities::BlockHitReact);
	BP_RemoveGameplayEffectFromOwnerWithGrantedTags(TagContainer, 1);
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

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName(""),	// Task Instance Name (can be empty)
		GuardMontage,
		1.f,
		FName("None"),	// Start Section
		false,	// bStopWhenAbilityEnds
		1.0f	// AnimRootMotionTranslationScale
	);

	if (MontageTask)
	{
		MontageTask->OnCompleted.AddDynamic(this, &UGP_Secondary::OnMontageCompleted);			// 애니메이션 몽타주가 정상적으로 종료되었을 때
		MontageTask->OnBlendOut.AddDynamic(this, &UGP_Secondary::OnMontageBlendOut);			// 애니메이션이 다음 애니메이션에 전환되는 타이밍 떄
		MontageTask->OnInterrupted.AddDynamic(this, &UGP_Secondary::OnMontageInterrupted);		// 몽타주가 중간에 강제로 종료되었을 때
		MontageTask->OnCancelled.AddDynamic(this, &UGP_Secondary::OnMontageCancelled);			// Ability 취소로 인해 몽타주가 취소된 경우

		MontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UGP_Secondary::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGP_Secondary::OnMontageBlendOut()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGP_Secondary::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGP_Secondary::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGP_Secondary::WaitForGameplayEvent(FGameplayTag EventTag)
{
	WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, 
		EventTag, 
		/* OptionalExternalTarget (ASC to listen on) */ nullptr,	// 다른 액터에서 발생하는 이벤트를 수신하도록 지정할 수 있다.
		/* OnlyTriggerOnce */ false,								// 첫 번째 이벤트가 수신된 후 종료할지(true) 아니면 여러 이벤트를 계속 수신할 지(false)
		/* OnlyMatchExact */ true									// 정확히 일치하는 태그만 이벤트를 트리거할지(true), 아니면 중첩된 태그도 트리거할지(false)
	);

	if (WaitTask)
	{
		WaitTask->EventReceived.AddDynamic(this, &UGP_Secondary::OnEventReceived);
		WaitTask->ReadyForActivation();
	}
}

void UGP_Secondary::OnEventReceived(FGameplayEventData Payload)
{
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
}
