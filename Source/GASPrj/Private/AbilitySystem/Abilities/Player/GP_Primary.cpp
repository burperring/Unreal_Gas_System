
#include "AbilitySystem/Abilities/Player/GP_Primary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/GP_BaseCharacter.h"
#include "GameplayTags/GPTags.h"
#include "Utils/GP_AbilitySystemBlueprintLibrary.h"

UGP_Primary::UGP_Primary()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGP_Primary::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
								  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (bDrawDebugs)
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Primary Active!"));

	ApplyBlockHitReactGE();
	PlayMontageFlipFlop();
	WaitForGameplayEvent(GPTags::Events::Player::Primary);
}

void UGP_Primary::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(GPTags::GPAbilities::BlockHitReact);
	BP_RemoveGameplayEffectFromOwnerWithGrantedTags(TagContainer, 1);

	if (MontageTask != nullptr) MontageTask->EndTask();
	if (WaitTask != nullptr) WaitTask->EndTask();
}

void UGP_Primary::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGP_Primary::OnMontageBlendOut()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGP_Primary::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGP_Primary::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGP_Primary::OnEventReceived(FGameplayEventData Payload)
{
	// 여기서 특정 어빌리티 능력에 대한 대기 이벤트를 실행시켜 준다.
	// 여기서 Payload 데이터에 접근할 수 있다.
	if (bDrawDebugs)
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Gameplay Primary Event Received!"));

	HitBoxOerlapApply();
}

void UGP_Primary::ApplyBlockHitReactGE()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!IsValid(ASC)) return;

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(BlockHitReactEffect, GetAbilityLevel(), ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UGP_Primary::HitBoxOerlapApply()
{
	TArray<AActor*> OverlapActors{};
	OverlapActors = UGP_AbilitySystemBlueprintLibrary::HitBoxOerlap(
		GetAvatarActorFromActorInfo(),
		HitBoxRadius,
		HitBoxForwardOffset,
		HitBoxElevationOffset,
		bDrawDebugs
	);
	
	SendHitReactEventToActors(OverlapActors);
	ApplyDamageEventToActors(OverlapActors);
}

void UGP_Primary::SendHitReactEventToActors(const TArray<AActor*>& HitActors) const
{
	for (AActor* HitActor : HitActors)
	{
		FGameplayEventData Payload;
		Payload.Instigator = GetAvatarActorFromActorInfo();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, GPTags::Events::Enemy::HitReact, Payload);
	}
}

void UGP_Primary::ApplyDamageEventToActors(const TArray<AActor*>& HitActors) const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!IsValid(ASC)) return;
	
	for (AActor* const OverlapActor : HitActors)
	{
		AGP_BaseCharacter* BaseCharacter = Cast<AGP_BaseCharacter>(OverlapActor);
		if (BaseCharacter == nullptr) continue;
		
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(PlayerDamageEffect, GetAbilityLevel(), ContextHandle);
		ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), BaseCharacter->GetAbilitySystemComponent());
	}
}

void UGP_Primary::PlayMontageFlipFlop()
{
	if (Montage_1 == nullptr || Montage_2 == nullptr)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName(""),	// Task Instance Name (can be empty)
		bFlip ? Montage_1 : Montage_2,
		1.f,
		FName("None"),	// Start Section
		true,	// bStopWhenAbilityEnds
		1.0f	// AnimRootMotionTranslationScale
	);

	if (MontageTask)
	{
		MontageTask->OnCompleted.AddDynamic(this, &UGP_Primary::OnMontageCompleted);		// 애니메이션 몽타주가 정상적으로 종료되었을 때
		MontageTask->OnBlendOut.AddDynamic(this, &UGP_Primary::OnMontageBlendOut);			// 애니메이션이 다음 애니메이션에 전환되는 타이밍 떄
		MontageTask->OnInterrupted.AddDynamic(this, &UGP_Primary::OnMontageInterrupted);	// 몽타주가 중간에 강제로 종료되었을 때
		MontageTask->OnCancelled.AddDynamic(this, &UGP_Primary::OnMontageCancelled);		// Ability 취소로 인해 몽타주가 취소된 경우

		MontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	bFlip = !bFlip;
}

void UGP_Primary::WaitForGameplayEvent(FGameplayTag EventTag)
{
	WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, 
		EventTag, 
		/* OptionalExternalTarget (ASC to listen on) */ nullptr,	// 다른 액터에서 발생하는 이벤트를 수신하도록 지정할 수 있다.
		/* OnlyTriggerOnce */ true,									// 첫 번째 이벤트가 수신된 후 종료할지(true) 아니면 여러 이벤트를 계속 수신할 지(false)
		/* OnlyMatchExact */ true									// 정확히 일치하는 태그만 이벤트를 트리거할지(true), 아니면 중첩된 태그도 트리거할지(false)
	);

	if (WaitTask)
	{
		WaitTask->EventReceived.AddDynamic(this, &UGP_Primary::OnEventReceived);
		WaitTask->ReadyForActivation();
	}
}
