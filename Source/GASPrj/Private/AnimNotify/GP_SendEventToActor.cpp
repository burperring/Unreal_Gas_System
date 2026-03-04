
#include "AnimNotify/GP_SendEventToActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Character/GP_PlayerCharacter.h"



void UGP_SendEventToActor::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AGP_BaseCharacter* Character = Cast<AGP_BaseCharacter>(MeshComp->GetOwner());
	if (Character == nullptr) return;
	if (!IsValid(Character->GetAbilitySystemComponent())) return;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Character, EventTag, EventData);
}
