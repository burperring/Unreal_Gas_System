
#include "AbilitySystem/GP_AttributeSet.h"
#include "Net/UnrealNetwork.h"

void UGP_AttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxMana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME(ThisClass, bAttributeInitialized);
}

void UGP_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (!bAttributeInitialized)
	{
		bAttributeInitialized = true;
		OnAttributesInitialized.Broadcast();
	}
}

void UGP_AttributeSet::OnRep_AttributeInitialized()
{
	if (bAttributeInitialized)
	{
		OnAttributesInitialized.Broadcast();
	}
}

void UGP_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	// GAS에는 예측 기능이 내장되어 있으며, 로컬에서 변경하고 GAS를 사용하여 서버 측에서 변경 및 복제 처리를 할 수 있다.
	// 하지만 GAS 시스템과 제대로 연동되려면 아래와 같은 매크로가 필요하다.
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldValue);
}

void UGP_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldValue);
}

void UGP_AttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Mana, OldValue);
}

void UGP_AttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxMana, OldValue);
}
