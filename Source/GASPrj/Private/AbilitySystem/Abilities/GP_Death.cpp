
#include "AbilitySystem/Abilities/GP_Death.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/GP_BaseCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayTags/GPTags.h"


UGP_Death::UGP_Death()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGP_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (bDrawDebugs)
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Activate Death!!"));
	
	PlayDeathMontage();
	ApplyDeathEffect();
}

void UGP_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGP_Death::OnMontageCompleted()
{
	RespawnCharacter();
}

void UGP_Death::OnMontageBlendOut()
{
}

void UGP_Death::OnMontageInterrupted()
{
	RespawnCharacter();
}

void UGP_Death::OnMontageCancelled()
{
	RespawnCharacter();
}

void UGP_Death::PlayDeathMontage()
{
	if (DeathMontage == nullptr)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	AGP_BaseCharacter* Character = Cast<AGP_BaseCharacter>(GetAvatarActorFromActorInfo());
	if (Character == nullptr) return;
	Character->GetMesh()->GetAnimInstance()->StopAllMontages(0.f);

	PlayMontageAndWait(DeathMontage);
}

void UGP_Death::ApplyDeathEffect() const
{
	// Gameplay Effect를 통한 Death Tag 생성(GPTags::Status::Death)
	checkf(IsValid(DeathEffect), TEXT("DeathEffect not set."));
	if (!IsValid(DeathEffect)) return;
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!IsValid(ASC)) return;

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DeathEffect, 1.f, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get()); 
}

void UGP_Death::RespawnCharacter()
{
	if (!GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(GPTags::Status::Death)) return;

	// 사망 상태 태그 제거
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(GPTags::Status::Death);
	BP_RemoveGameplayEffectFromOwnerWithGrantedTags(TagContainer, 1);
	
	ResetDeathCharacter();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGP_Death::ResetDeathCharacter() const
{
	AGP_BaseCharacter* BaseCharacter = Cast<AGP_BaseCharacter>(GetAvatarActorFromActorInfo());
	if (BaseCharacter == nullptr) return;
	BaseCharacter->HandleRespawn();

	// 서버에서만 호출될 수 있도록 한다.
	if (!HasAuthority(&CurrentActivationInfo)) return;
	BaseCharacter->ResetAttributes();

	// Respawn Character 위치 세팅
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	FVector StartLocation = GameMode->FindPlayerStart(BaseCharacter->GetController())->GetActorLocation();
	BaseCharacter->SetActorLocation(StartLocation);
}

