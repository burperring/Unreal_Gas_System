
#include "AbilitySystem/Abilities/Player/GP_PlayerImpactCues.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameplayTags/GPTags.h"


UGP_PlayerImpactCues::UGP_PlayerImpactCues()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGP_PlayerImpactCues::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	WaitForGameplayEvent(WaitHitReactTask, GPTags::Events::Player::HitReact, &UGP_PlayerImpactCues::OnHitReactEventReceived);
	WaitForGameplayEvent(WaitDeathTask, GPTags::GPAbilities::Death, &UGP_PlayerImpactCues::OnDeathEventReceived);
}

void UGP_PlayerImpactCues::OnHitReactEventReceived(FGameplayEventData Payload)
{
}

void UGP_PlayerImpactCues::OnDeathEventReceived(FGameplayEventData Payload)
{
}

void UGP_PlayerImpactCues::WaitForGameplayEvent(TObjectPtr<UAbilityTask_WaitGameplayEvent>& WaitTask, FGameplayTag EventTag,
	void(UGP_PlayerImpactCues::* CallbackFunc)(FGameplayEventData))
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
		WaitTask->EventReceived.AddDynamic(this, CallbackFunc);
		WaitTask->ReadyForActivation();
	}
}
