
#include "AnimNotify/GP_MeleeAttackNotifyState.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "KismetTraceUtils.h"
#include "Character/GP_PlayerCharacter.h"
#include "GameplayTags/GPTags.h"
#include "Kismet/KismetMathLibrary.h"


void UGP_MeleeAttackNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                            float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!IsValid(MeshComp)) return;
	if (!IsValid(MeshComp->GetOwner())) return;

	TArray<FHitResult> Hits = PerformSphereTrace(MeshComp);
	SendEventsToActors(MeshComp, Hits);
}

TArray<FHitResult> UGP_MeleeAttackNotifyState::PerformSphereTrace(USkeletalMeshComponent* MeshComp) const
{
	TArray<FHitResult> OutHits;

	const FTransform SocketTransform = MeshComp->GetSocketTransform(SocketName);
	const FVector Start = SocketTransform.GetLocation();
	const FVector ExtendedSocketDirection = UKismetMathLibrary::GetForwardVector(SocketTransform.GetRotation().Rotator()) * SocketExtenstionOffset;
	const FVector End = Start - ExtendedSocketDirection;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(MeshComp->GetOwner());

	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);
	
	UWorld* World = GEngine->GetWorldFromContextObject(MeshComp, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World)) return OutHits;
	
	bool const bHit = World->SweepMultiByChannel(
		OutHits,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(SphereTraceRadius),
		Params,
		ResponseParams
	);

	if (bDrawDebugs)
	{
		DrawDebugSphereTraceMulti(
			World,
			Start,
			End,
			SphereTraceRadius,
			EDrawDebugTrace::ForDuration,
			bHit,
			OutHits,
			FColor::Red,
			FColor::Green,
			5.f
		);
	}
	
	return OutHits;
}

void UGP_MeleeAttackNotifyState::SendEventsToActors(USkeletalMeshComponent* MeshComp, const TArray<FHitResult>& Hits) const
{
	for (const FHitResult& Hit : Hits)
	{
		AGP_PlayerCharacter* PC = Cast<AGP_PlayerCharacter>(Hit.GetActor());
		if (PC == nullptr) continue;
		if (!PC->IsAlive()) continue;

		UAbilitySystemComponent* ASC = PC->GetAbilitySystemComponent();
		if (!IsValid(ASC)) continue;

		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddHitResult(Hit);
		
		FGameplayEventData Payload;
		Payload.Target = PC;
		Payload.Instigator = MeshComp->GetOwner();
		Payload.ContextHandle = ContextHandle;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), GPTags::Events::Enemy::MeleeTraceHit, Payload);
	}
}
