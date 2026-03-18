
#include "Character/GP_EnemyCharacter.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "AbilitySystem/GP_AbilitySystemComponent.h"
#include "AbilitySystem/GP_AttributeSet.h"
#include "GameplayTags/GPTags.h"
#include "Kismet/KismetMathLibrary.h"


AGP_EnemyCharacter::AGP_EnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UGP_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	// 적에게 대한 게임플레이 효과를 최대한으로 복제할 필요가 없다
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UGP_AttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AGP_EnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AGP_EnemyCharacter::GetAttributeSet() const
{
	return AttributeSet;
}

void AGP_EnemyCharacter::RotateToTarget(AActor* Target)
{
	if (!Target) return;

	CurrentTarget = Target;

	if (GetWorldTimerManager().IsTimerActive(RotationTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(RotationTimerHandle);
		GetWorldTimerManager().ClearTimer(StopTimerHandle);
	}
	
	GetWorldTimerManager().SetTimer(RotationTimerHandle, this, &AGP_EnemyCharacter::UpdateRotationToTarget, 0.01f, true);
	GetWorldTimerManager().SetTimer(StopTimerHandle, this, &AGP_EnemyCharacter::UpdateRotationToTarget, TimerLength, false);
}

void AGP_EnemyCharacter::UpdateRotationToTarget()
{
	if (!CurrentTarget)
	{
		GetWorldTimerManager().ClearTimer(RotationTimerHandle);
		return;
	}

	FVector ActorLocation = GetActorLocation();
	FVector TargetLocation = CurrentTarget->GetActorLocation();
	FRotator CurrentRotation = GetActorRotation();
	
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(ActorLocation, TargetLocation);
    
	// Z축(Yaw)만 부드럽게 회전시키고 싶을 경우 (바닥에 서 있는 캐릭터 기준)
	FRotator DesiredRotation = FRotator(0.f, TargetRotation.Yaw, 0.f);

	// 보간 속도 (숫자가 높을수록 빠름)
	float InterpSpeed = 5.0f;
	float DeltaTime = 0.01f; // 타이머 주기에 맞춤

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaTime, InterpSpeed);

	SetActorRotation(NewRotation);

	// 오차 범위 내로 회전이 완료되었는지 확인 (선택 사항: 완료 시 타이머 종료)
	if (NewRotation.Equals(DesiredRotation, 1.0f))
	{
		GetWorldTimerManager().ClearTimer(RotationTimerHandle);
	}
}

void AGP_EnemyCharacter::StopTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(RotationTimerHandle);
}

void AGP_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;

	ASC->InitAbilityActorInfo(this, this);
	OnASCInitialized.Broadcast(ASC, GetAttributeSet());

	if (!HasAuthority()) return;

	GiveStartupAbilities();
	InitializeAttributes();
	
	SetAttributeValueChange();
}

void AGP_EnemyCharacter::HandleDeath()
{
	if (IsAlive())
		ActivateAbility(GPTags::GPAbilities::Death);

	Super::HandleDeath();

	AAIController* AIController = GetController<AAIController>();
	if (!IsValid(AIController)) return;

	AIController->StopMovement();
}
