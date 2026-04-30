
#include "AbilitySystem/Abilities/GP_GameplayAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UGP_GameplayAbility::UGP_GameplayAbility()
{
}

void UGP_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (bDrawDebugs)
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Gameplay Ability Activate!"));
}

void UGP_GameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (MontageTask != nullptr) MontageTask->EndTask();
	if (WaitTask != nullptr) WaitTask->EndTask();
}

// GAS System의 주요 특징
// RPC를 사용하지 않아도 자동으로 서버 동기화를 해준다
void UGP_GameplayAbility::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGP_GameplayAbility::OnMontageBlendIn()
{
}

void UGP_GameplayAbility::OnMontageBlendOut()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGP_GameplayAbility::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGP_GameplayAbility::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGP_GameplayAbility::OnEventReceived(FGameplayEventData Payload)
{
	// 여기서 특정 어빌리티 능력에 대한 대기 이벤트를 실행시켜 준다.
	// 여기서 Payload 데이터에 접근할 수 있다.

	if (bDrawDebugs)
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Gameplay Ability Wait Event Received!"));
}


// 자주 사용되는 Task 기본적으로 구현
// 관련된 함수의 경우 필요에 따라 override를 통해 수정
void UGP_GameplayAbility::PlayMontageAndWait(TObjectPtr<UAnimMontage> Montage, float Rate, FName StartSection,
	bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScale)
{
	if (Montage == nullptr)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName(""),						// Task Instance Name (can be empty)
		Montage,
		Rate,
		StartSection,					// Start Section
		bStopWhenAbilityEnds,			// bStopWhenAbilityEnds
		AnimRootMotionTranslationScale	// AnimRootMotionTranslationScale
	);

	if (MontageTask)
	{
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);		// 애니메이션 몽타주가 정상적으로 종료되었을 때
		MontageTask->OnBlendedIn.AddDynamic(this, &ThisClass::OnMontageBlendIn);		// 현재 애니메이션 몽타주가 실행되는 타이밍 때
		MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageBlendOut);		// 애니메이션이 다음 애니메이션에 전환되는 타이밍 떄
		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);	// 몽타주가 중간에 강제로 종료되었을 때
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);		// Ability 취소로 인해 몽타주가 취소된 경우

		MontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UGP_GameplayAbility::WaitForGameplayEvent(FGameplayTag EventTag, AActor* OptionalExternalTarget,
	bool bOnlyTriggerOnce, bool bOnlyMatchExact)
{
	WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
	this, 
	EventTag, 
	OptionalExternalTarget,		// 다른 액터에서 발생하는 이벤트를 수신하도록 지정할 수 있다.
	bOnlyTriggerOnce,			// 첫 번째 이벤트가 수신된 후 종료할지(true) 아니면 여러 이벤트를 계속 수신할 지(false)
	bOnlyMatchExact				// 정확히 일치하는 태그만 이벤트를 트리거할지(true), 아니면 중첩된 태그도 트리거할지(false)
	);

	if (WaitTask)
	{
		WaitTask->EventReceived.AddDynamic(this, &ThisClass::OnEventReceived);
		WaitTask->ReadyForActivation();
	}
}
