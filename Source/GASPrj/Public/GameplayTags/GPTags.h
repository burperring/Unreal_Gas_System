#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace GPTags
{
	namespace SetByCaller
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Projectile);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(MeleeTraceHit);
		
		namespace Player
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Secondary);
		}
	}
	
	namespace GPAbilities
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ActivateOnGiven);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Death);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(BlockHitReact);
		
		namespace Player
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Secondary);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tertiary);
		}

		namespace Enemy
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack);
		}
	}

	namespace Cooldown
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Secondary);
	}

	namespace Events
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(KillScored);
		
		namespace Player
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Secondary);
		}

		namespace Enemy
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(EndAttack);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(MeleeTraceHit);
		}
	}

	namespace Status
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Death);
	}
}