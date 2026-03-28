
#include "AbilitySystem/Abilities/Player/GP_PlayerImpactCues.h"

#include "AbilitySystemComponent.h"
#include "GameplayCueFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/GP_PlayerCharacter.h"
#include "GameplayTags/GPTags.h"


UGP_PlayerImpactCues::UGP_PlayerImpactCues()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGP_PlayerImpactCues::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	WaitForHitReactGameplayEvent(GPTags::Events::Player::HitReact);
	WaitForDeathGameplayEvent(GPTags::GPAbilities::Death);
}

void UGP_PlayerImpactCues::OnHitReactEventReceived(FGameplayEventData Payload)
{
	AGP_PlayerCharacter* PC = Cast<AGP_PlayerCharacter>(GetAvatarActorFromActorInfo());
	if (PC == nullptr) return;
	
	UAbilitySystemComponent* ASC = PC->GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;
	
	FGameplayCueParameters CueParams;
	CueParams.EffectContext = Payload.ContextHandle;
	CueParams.Instigator = Payload.Instigator->GetInstigator();
	CueParams.SourceObject = Payload.OptionalObject;
	
	ASC->ExecuteGameplayCue(GameplayHitReactCueTag, CueParams);
}

void UGP_PlayerImpactCues::OnDeathEventReceived(FGameplayEventData Payload)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Player Death Receive"));

	AGP_PlayerCharacter* PC = Cast<AGP_PlayerCharacter>(GetAvatarActorFromActorInfo());
	if (PC == nullptr) return;
	
	UAbilitySystemComponent* ASC = PC->GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;

	FGameplayCueParameters CueParams;
	CueParams.EffectContext = Payload.ContextHandle;
	CueParams.Instigator = Payload.Instigator->GetInstigator();
	CueParams.SourceObject = Payload.OptionalObject;

	ASC->ExecuteGameplayCue(GameplayBurstImpactTag, CueParams);

	FGameplayTag AbilityTag{GPTags::GPAbilities::Death};
	ASC->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer());
}

void UGP_PlayerImpactCues::WaitForHitReactGameplayEvent(FGameplayTag EventTag)
{
	WaitHitReactTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, 
		EventTag, 
		/* OptionalExternalTarget (ASC to listen on) */ nullptr,	// 다른 액터에서 발생하는 이벤트를 수신하도록 지정할 수 있다.
		/* OnlyTriggerOnce */ false,								// 첫 번째 이벤트가 수신된 후 종료할지(true) 아니면 여러 이벤트를 계속 수신할 지(false)
		/* OnlyMatchExact */ true									// 정확히 일치하는 태그만 이벤트를 트리거할지(true), 아니면 중첩된 태그도 트리거할지(false)
	);

	if (WaitHitReactTask)
	{
		WaitHitReactTask->EventReceived.AddDynamic(this, &UGP_PlayerImpactCues::OnHitReactEventReceived);
		WaitHitReactTask->ReadyForActivation();
	}
}

void UGP_PlayerImpactCues::WaitForDeathGameplayEvent(FGameplayTag EventTag)
{
	WaitDeathTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, 
		EventTag, 
		/* OptionalExternalTarget (ASC to listen on) */ nullptr,	// 다른 액터에서 발생하는 이벤트를 수신하도록 지정할 수 있다.
		/* OnlyTriggerOnce */ false,								// 첫 번째 이벤트가 수신된 후 종료할지(true) 아니면 여러 이벤트를 계속 수신할 지(false)
		/* OnlyMatchExact */ true									// 정확히 일치하는 태그만 이벤트를 트리거할지(true), 아니면 중첩된 태그도 트리거할지(false)
	);

	if (WaitDeathTask)
	{
		WaitDeathTask->EventReceived.AddDynamic(this, &UGP_PlayerImpactCues::OnDeathEventReceived);
		WaitDeathTask->ReadyForActivation();
	}
}
