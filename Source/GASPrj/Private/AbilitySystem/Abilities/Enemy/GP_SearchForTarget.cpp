
#include "AbilitySystem/Abilities/Enemy/GP_SearchForTarget.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/GP_EnemyCharacter.h"
#include "GameplayTags/GPTags.h"
#include "Tasks/AITask_MoveTo.h"
#include "Utils/GP_AbilitySystemBlueprintLibrary.h"


UGP_SearchForTarget::UGP_SearchForTarget()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UGP_SearchForTarget::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	OwningEnemy = Cast<AGP_EnemyCharacter>(GetAvatarActorFromActorInfo());
	check (OwningEnemy.IsValid());
	OwningAIController = Cast<AAIController>(OwningEnemy->GetController());
	check (OwningAIController.IsValid());

	StartSearch();
	
	WaitForGameplayEvent(GPTags::Events::Enemy::EndAttack);
}

void UGP_SearchForTarget::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	if (SearchDelayTask != nullptr) SearchDelayTask->EndTask();
	if (AttackDelayTask != nullptr) AttackDelayTask->EndTask();
	if (MoveToLocationOrActorTask != nullptr) MoveToLocationOrActorTask->EndTask();
}

void UGP_SearchForTarget::StartSearch()
{
	if (bDrawDebugs) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("GP_SearchForTarget::StartSearch")));
	if (!OwningEnemy.IsValid()) return;
	
	const float SearchDelay = FMath::RandRange(OwningEnemy->GetEnemyMinAttackDelay(), OwningEnemy->GetEnemyMaxAttackDelay());
	SearchDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, SearchDelay);
	SearchDelayTask->OnFinish.AddDynamic(this, &UGP_SearchForTarget::Search);
	SearchDelayTask->Activate();
}

void UGP_SearchForTarget::Search()
{
	const FVector SearchOrigin = GetAvatarActorFromActorInfo()->GetActorLocation();
	// Player Tag를 가진 Actor중 가장 가까운 Actor 탐색
	FClosestActorWithTagResult ClosestActorResult = UGP_AbilitySystemBlueprintLibrary::FindClosestActorWithTag(
		GetAvatarActorFromActorInfo(), SearchOrigin, GPTypeTags::Player, OwningEnemy->GetSearchRange());

	TargetBaseCharacter = Cast<AGP_BaseCharacter>(ClosestActorResult.Actor);
	if (!TargetBaseCharacter.IsValid())
	{
		StartSearch();	// 유효하지 않는 경우 재탐색
		return;
	}

	if (TargetBaseCharacter->IsAlive())
	{
		MoveTargetAndAttack();	// 이동 후 공격
	}
	else
	{
		StartSearch();			// 재탐색
	}
}

void UGP_SearchForTarget::MoveTargetAndAttack()
{
	if (!OwningEnemy.IsValid() || !OwningAIController.IsValid()) return;
	if (!TargetBaseCharacter.IsValid()) return;
	if (!OwningEnemy->IsAlive())
	{
		StartSearch();	// 캐릭터 사망 시 리스폰으로 인한 재탐색
		return;
	}

	MoveToLocationOrActorTask = UAITask_MoveTo::AIMoveTo(
		OwningAIController.Get(),
		FVector(),
		TargetBaseCharacter.Get(),
		OwningEnemy->GetEnemyAcceptanceRadius()
	);

	MoveToLocationOrActorTask->OnMoveTaskFinished.AddUObject(this, &UGP_SearchForTarget::AttackTarget);
	MoveToLocationOrActorTask->ConditionalPerformMove();
}

void UGP_SearchForTarget::AttackTarget(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController)
{
	if (!OwningEnemy.IsValid()) return;

	OwningEnemy->RotateToTarget(TargetBaseCharacter.Get());

	// 공격에 필요한 딜레이만큼 대기
	AttackDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, OwningEnemy->GetTimerLength());
	AttackDelayTask->OnFinish.AddDynamic(this, &UGP_SearchForTarget::Attack);
	AttackDelayTask->Activate();
}

void UGP_SearchForTarget::Attack()
{
	// 공격 Tag 전달
	const FGameplayTag AttackTag{GPTags::GPAbilities::Enemy::Attack};
	GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(AttackTag.GetSingleTagContainer());
}

void UGP_SearchForTarget::OnEventReceived(FGameplayEventData Payload)
{
	if (OwningEnemy.IsValid() && !OwningEnemy->GetEnemyIsLaunched())
		StartSearch();
}
