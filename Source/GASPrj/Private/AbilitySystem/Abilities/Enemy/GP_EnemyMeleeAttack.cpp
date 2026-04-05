
#include "AbilitySystem/Abilities/Enemy/GP_EnemyMeleeAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/GP_PlayerCharacter.h"
#include "GameplayTags/GPTags.h"
#include "Particles/ParticleSystem.h"
#include "Utils/GP_AbilitySystemBlueprintLibrary.h"


void UGP_EnemyMeleeAttack::BeginDestroy()
{
	Super::BeginDestroy();

	if (WaitTask != nullptr)
	{
		WaitTask->EventReceived.RemoveAll(this);
	}
}

void UGP_EnemyMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (bDrawDebugs)
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("EnemyMeleeAttack Active!"));
	
	if (IsEnemyDeath()) return;

	WaitForGameplayEvent(GPTags::Events::Enemy::MeleeTraceHit);
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

void UGP_EnemyMeleeAttack::WaitForGameplayEvent(FGameplayTag EventTag)
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
		WaitTask->EventReceived.AddDynamic(this, &UGP_EnemyMeleeAttack::OnEventReceived);
		WaitTask->ReadyForActivation();
	}
}
