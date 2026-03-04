
#include "AbilitySystem/GP_AbilitySystemComponent.h"

#include "GameplayTags/GPTags.h"


UGP_AbilitySystemComponent::UGP_AbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UGP_AbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	HandleAutoActivateAbility(AbilitySpec);
}

void UGP_AbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	// 클라에서 생성된 능력은 서버에서 제대로 실행되지 않아 문제가 발생할 수 있다.
	// 이러한 부분을 해결하기 위해 활성화된 능력을 복제해줘야 한다.

	// 이미 제거된 능력을 사용하려고 시도하는 것을 방지해야 한다.
	FScopedAbilityListLock ActiveScopeLock(*this);

	// 모든 능력을 순회하면서 해등 능력이 유효한지 확인하고 활성화를 시도한다.
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		HandleAutoActivateAbility(AbilitySpec);
	}
}

void UGP_AbilitySystemComponent::HandleAutoActivateAbility(const FGameplayAbilitySpec& AbilitySpec)
{
	if (!IsValid(AbilitySpec.Ability)) return;
	
	for (const FGameplayTag& Tag : AbilitySpec.Ability->GetAssetTags())
	{
		if (Tag.MatchesTagExact(GPTags::GPAbilities::ActivateOnGiven))
		{
			// 자동으로 능력이 활성화 될 수 있도록 게임 플레이 태그를 추가할 수 있다.
			TryActivateAbility(AbilitySpec.Handle);
			return;
		}
	}
}

