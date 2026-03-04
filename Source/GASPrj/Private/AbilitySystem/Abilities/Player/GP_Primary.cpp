
#include "AbilitySystem/Abilities/Player/GP_Primary.h"

#include "Engine/OverlapResult.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameplayTags/GPTags.h"

void UGP_Primary::BeginDestroy()
{
	Super::BeginDestroy();

	if (MontageTask != nullptr)
	{
		MontageTask->OnCompleted.RemoveAll(this);
		MontageTask->OnBlendOut.RemoveAll(this);
		MontageTask->OnInterrupted.RemoveAll(this);
		MontageTask->OnCancelled.RemoveAll(this);
	}
	if (WaitTask != nullptr)
	{
		WaitTask->EventReceived.RemoveAll(this);
	}
}

void UGP_Primary::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
								  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (bDrawDebugs)
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Primary Active"), true, true, FLinearColor(0.0f, 0.66f, 1.0f, 1.0f), 2.0f);

	PlayMontageFlipFlop();

	WaitForGameplayEvent(GPTags::Events::Player::Primary);
}

void UGP_Primary::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (bDrawDebugs)
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Primary End"), true, true, FLinearColor(0.0f, 0.66f, 1.0f, 1.0f), 2.0f);
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
	UE_LOG(LogTemp, Warning, TEXT("Gameplay Event Received! Tag: %s"), *Payload.EventTag.ToString());
	HitBoxOerlapTest();
}

void UGP_Primary::HitBoxOerlapTest()
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());

	// 아바타 액터에 대한 오버랩 감지 제거
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActors(ActorsToIgnore);

	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);

	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(HitBoxRadius);

	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector() * HitBoxForwardOffset;
	const FVector HitBoxLocation = GetAvatarActorFromActorInfo()->GetActorLocation() + Forward + FVector(0.f, 0.f, HitBoxElevationOffset);
	
	GetWorld()->OverlapMultiByChannel(OverlapResults, HitBoxLocation,
		FQuat::Identity, ECC_Visibility, Sphere, CollisionParams, ResponseParams);

	if (bDrawDebugs)
	{
		DrawDebugSphere(GetWorld(), HitBoxLocation, HitBoxRadius, 16, FColor::Red, false, 3.f);

		for (const FOverlapResult& Result : OverlapResults)
		{
			if (IsValid(Result.GetActor()))
			{
				FVector DebugLocation = Result.GetActor()->GetActorLocation();
				DebugLocation.Z += 100.f;
				DrawDebugSphere(GetWorld(), DebugLocation, 30.f, 10, FColor::Green, false, 3.f);
			}
		}
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
		MontageTask->OnCompleted.AddDynamic(this, &UGP_Primary::OnMontageCompleted);
		MontageTask->OnBlendOut.AddDynamic(this, &UGP_Primary::OnMontageBlendOut);
		MontageTask->OnInterrupted.AddDynamic(this, &UGP_Primary::OnMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &UGP_Primary::OnMontageCancelled);

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
		/* OptionalExternalTarget (ASC to listen on) */ nullptr, 
		/* OnlyTriggerOnce */ true, 
		/* OnlyMatchExact */ true
	);

	if (WaitTask)
	{
		WaitTask->EventReceived.AddDynamic(this, &UGP_Primary::OnEventReceived);
		WaitTask->ReadyForActivation();
	}
}
