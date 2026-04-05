
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
	FClosestActorWithTagResult ClosestActorResult = UGP_AbilitySystemBlueprintLibrary::FindClosestActorWithTag(GetAvatarActorFromActorInfo(), SearchOrigin, GPTypeTags::Player, OwningEnemy->GetSearchRange());

	TargetBaseCharacter = Cast<AGP_BaseCharacter>(ClosestActorResult.Actor);
	if (!TargetBaseCharacter.IsValid())
	{
		StartSearch();
		return;
	}

	if (TargetBaseCharacter->IsAlive())
	{
		MoveTargetAndAttack();
	}
	else
	{
		StartSearch();
	}
}

void UGP_SearchForTarget::MoveTargetAndAttack()
{
	if (!OwningEnemy.IsValid() || !OwningAIController.IsValid()) return;
	if (!TargetBaseCharacter.IsValid()) return;
	if (!OwningEnemy->IsAlive())
	{
		StartSearch();
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
	
	AttackDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, OwningEnemy->GetTimerLength());
	AttackDelayTask->OnFinish.AddDynamic(this, &UGP_SearchForTarget::Attack);
	AttackDelayTask->Activate();
}

void UGP_SearchForTarget::Attack()
{
	const FGameplayTag AttackTag{GPTags::GPAbilities::Enemy::Attack};
	GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(AttackTag.GetSingleTagContainer());
}

void UGP_SearchForTarget::WaitForGameplayEvent(FGameplayTag EventTag)
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
		WaitTask->EventReceived.AddDynamic(this, &UGP_SearchForTarget::OnEventReceived);
		WaitTask->ReadyForActivation();
	}
}

void UGP_SearchForTarget::OnEventReceived(FGameplayEventData Payload)
{
	if (OwningEnemy.IsValid() && !OwningEnemy->GetEnemyIsLaunched())
		StartSearch();
}
