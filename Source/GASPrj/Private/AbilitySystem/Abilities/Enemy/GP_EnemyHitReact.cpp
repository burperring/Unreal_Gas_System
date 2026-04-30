
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
}

void UGP_EnemyHitReact::OnEventReceived(FGameplayEventData Payload)
{
	if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(GPTags::Status::Death)) return;
	
	CacheHitDirectionVectors(Payload.Instigator);

	EHitDirection HitDirection{UGP_AbilitySystemBlueprintLibrary::GetHitDirection(AvatarForwardVector, ToInstigator)};	// 충돌 방향에 따른
	FName SectionName{UGP_AbilitySystemBlueprintLibrary::GetHitDirectionName(HitDirection)};							// 충돌 몽타주 섹션 설정

	if (bDrawDebugs)
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Hit React Event : %s"), *SectionName.ToString()));

	if (HitMontage == nullptr)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	PlayMontageAndWait(HitMontage, 1.f, SectionName, true, 1.f);		// 몽타주 Task 실행
}

void UGP_EnemyHitReact::CacheHitDirectionVectors(const AActor* Instigator)
{
	// 현재 캐릭터와 Instigator 위치에 따른 충돌 위치 검사
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor == nullptr) return;
	
	AvatarForwardVector = AvatarActor->GetActorForwardVector();

	const FVector AvatarLocation = AvatarActor->GetActorLocation();
	const FVector InstigatorLocation = Instigator->GetActorLocation();

	ToInstigator = InstigatorLocation - AvatarLocation;
	ToInstigator.Normalize();
}
