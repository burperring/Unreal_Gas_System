
#include "AbilitySystem/Abilities/Enemy/GP_EnemyAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/GP_EnemyCharacter.h"
#include "GameplayTags/GPTags.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utils/GP_AbilitySystemBlueprintLibrary.h"


UGP_EnemyAttack::UGP_EnemyAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	bRetriggerInstancedAbility = true;
}

void UGP_EnemyAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PlayEnemyAttackMontage();

	if (HasAuthority(&CurrentActivationInfo))
	{
		SpawnProjectile();

		// 원거리인 경우 총 발싸와 동시에 GPTags::Events::Enemy::EndAttack 전송
		if (!bIsMelee) SendEndAttackEventTag();
	}
}

void UGP_EnemyAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 근거리인 경우 몽타주 실행 종료 이후 GPTags::Events::Enemy::EndAttack 전송
	if (bIsMelee) SendEndAttackEventTag();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGP_EnemyAttack::PlayEnemyAttackMontage()
{
	if (AttackMontageArray.Num() == 0)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	const auto& UAnimMontage = AttackMontageArray[FMath::RandRange(0, AttackMontageArray.Num() - 1)];

	PlayMontageAndWait(UAnimMontage);
}

void UGP_EnemyAttack::SpawnProjectile()
{
	if (EnemyProjectile == nullptr) return;

	AGP_EnemyCharacter* EnemyCharacter = Cast<AGP_EnemyCharacter>(GetAvatarActorFromActorInfo());
	
	FClosestActorWithTagResult ClosestActorResult = UGP_AbilitySystemBlueprintLibrary::FindClosestActorWithTag(
		GetAvatarActorFromActorInfo(),
		GetAvatarActorFromActorInfo()->GetActorLocation(),
		"Player",
		EnemyCharacter->GetEnemyAcceptanceRadius() + 100.f
	);

	if (!ClosestActorResult.Actor.IsValid()) return;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = EnemyCharacter;
	SpawnParams.Instigator = EnemyCharacter;
	
	const FTransform MuzzleTransform = EnemyCharacter->GetMesh()->GetSocketTransform(MuzzleSocketName);
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(MuzzleTransform.GetLocation(), ClosestActorResult.Actor->GetActorLocation());
	//FVector ForwardVector = MuzzleTransform.GetRotation().GetForwardVector();
	//ForwardVector.Z = 0.f;
	//const FRotator SpawnRotator = FRotationMatrix::MakeFromX(ForwardVector).Rotator();
	
	GetWorld()->SpawnActor<AActor>(
		EnemyProjectile,
		MuzzleTransform.GetLocation(),
		TargetRotation,
		SpawnParams
	);
}

void UGP_EnemyAttack::SendEndAttackEventTag()
{
	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActorFromActorInfo(), GPTags::Events::Enemy::EndAttack, Payload);
}