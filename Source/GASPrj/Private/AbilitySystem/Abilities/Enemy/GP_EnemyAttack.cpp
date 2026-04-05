
#include "AbilitySystem/Abilities/Enemy/GP_EnemyAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/GP_EnemyCharacter.h"
#include "GameplayTags/GPTags.h"


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

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName(""),		// Task Instance Name (can be empty)
		UAnimMontage,
		1.f,
		FName("None"),	// Start Section
		bIsMelee,		// bStopWhenAbilityEnds
		1.0f			// AnimRootMotionTranslationScale
	);

	if (MontageTask)
	{
		MontageTask->OnCompleted.AddDynamic(this, &UGP_EnemyAttack::OnMontageCompleted);		// 애니메이션 몽타주가 정상적으로 종료되었을 때
		MontageTask->OnInterrupted.AddDynamic(this, &UGP_EnemyAttack::OnMontageInterrupted);	// 몽타주가 중간에 강제로 종료되었을 때
		MontageTask->OnCancelled.AddDynamic(this, &UGP_EnemyAttack::OnMontageCancelled);		// Ability 취소로 인해 몽타주가 취소된 경우

		MontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UGP_EnemyAttack::SpawnProjectile()
{
	if (EnemyProjectile == nullptr) return;

	AGP_EnemyCharacter* EnemyCharacter = Cast<AGP_EnemyCharacter>(GetAvatarActorFromActorInfo());
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = EnemyCharacter;
	SpawnParams.Instigator = EnemyCharacter;
	
	const FTransform MuzzleTransform = EnemyCharacter->GetMesh()->GetSocketTransform(MuzzleSocketName);

	FVector ForwardVector = MuzzleTransform.GetRotation().GetForwardVector();
	ForwardVector.Z = 0.f;
	const FRotator SpawnRotator = FRotationMatrix::MakeFromX(ForwardVector).Rotator();
	
	GetWorld()->SpawnActor<AActor>(
		EnemyProjectile,
		MuzzleTransform.GetLocation(),
		SpawnRotator,
		SpawnParams
	);
}

void UGP_EnemyAttack::SendEndAttackEventTag()
{
	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActorFromActorInfo(), GPTags::Events::Enemy::EndAttack, Payload);
}

void UGP_EnemyAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGP_EnemyAttack::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGP_EnemyAttack::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
