
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
	WaitForGameplayEvent(GPTags::Events::Player::Primary, nullptr, true, true);
}

void UGP_Primary::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(GPTags::GPAbilities::BlockHitReact);
	BP_RemoveGameplayEffectFromOwnerWithGrantedTags(TagContainer, 1);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
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
	OverlapActors = UGP_AbilitySystemBlueprintLibrary::HitBoxOerlap(	// 충돌 검사
		GetAvatarActorFromActorInfo(),
		HitBoxRadius,
		HitBoxForwardOffset,
		HitBoxElevationOffset,
		bDrawDebugs
	);
	
	SendHitReactEventToActors(OverlapActors);	// 충돌 반응 애니메이션 Event
	ApplyDamageEventToActors(OverlapActors);	// 충돌 데미지 Event
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

	PlayMontageAndWait(bFlip ? Montage_1 : Montage_2);

	bFlip = !bFlip;
}