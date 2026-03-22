
#include "GASPrj/Public/GameplayTags/GPTags.h"

namespace GPTags
{
	namespace SetByCaller
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Projectile, "GPTags.SetByCaller.Projectile", "Tag for Set by Caller Magnitude for Projectiles")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MeleeTraceHit, "GPTags.SetByCaller.MeleeTraceHit", "Tag for Set by Caller Magnitude for MeleeTraceHit")
	}
	
	namespace GPAbilities
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateOnGiven, "GPTags.GPAbilities.ActivateOnGiven", "Tag for the Abilities that should activate immediately once given")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Death, "GPTags.GPAbilities.Death", "Tag for the Character Death")
		
		namespace Player
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "GPTags.GPAbilities.Player.Primary", "Tag for the Primary Ability")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Secondary, "GPTags.GPAbilities.Player.Secondary", "Tag for the Secondary Ability")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tertiary, "GPTags.GPAbilities.Player.Tertiary", "Tag for the Tertiary Ability")
		}

		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "GPTags.GPAbilities.Enemy.HitReact", "Tag for the Enemy Hit React Ability")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attack, "GPTags.GPAbilities.Enemy.Attack", "Tag for the Enemy Attack Ability")
		}
	}

	namespace Events
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(KillScored, "GPTags.Events.KillScored", "Tag for the KillScored Event")
		
		namespace Player
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "GPTags.Events.Player.HitReact", "Tag for the Player HitReact")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "GPTags.Events.Player.Primary", "Tag for the Player Gameplay Primary Event")
		}

		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "GPTags.Events.Enemy.HitReact", "Tag for the Enemy HitReact")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(EndAttack, "GPTags.Events.Enemy.EndAttack", "Tag for the Enemy EndAttack")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(MeleeTraceHit, "GPTags.Events.Enemy.MeleeTraceHit", "Tag for the Enemy Melee Trace Hit")
		}
	}

	namespace Status
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Death, "GPTags.Status.Death", "Set Death Status")
	}
}

// C++ 코드에서 태그를 생성하는 이유
// C++ 코드에서 참조가 필요 없는 경우 언리얼 에디터를 통해 생성하면 되지만
// C++ 코드를 통해 참조가 이루어질 경우 C++ 코드에서 태그를 생성해야 접근할 수 있다.