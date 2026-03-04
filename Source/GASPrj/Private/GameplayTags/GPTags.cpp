
#include "GASPrj/Public/GameplayTags/GPTags.h"

namespace GPTags
{
	namespace GPAbilities
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateOnGiven, "GPTags.GPAbilities.ActivateOnGiven", "Tag for the Abilities that should activate immediately once given")
		
		namespace Player
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "GPTags.GPAbilities.Player.Primary", "Tag for the Primary Ability")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Secondary, "GPTags.GPAbilities.Player.Secondary", "Tag for the Secondary Ability")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tertiary, "GPTags.GPAbilities.Player.Tertiary", "Tag for the Tertiary Ability")
		}

		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "GPTags.GPAbilities.Enemy.HitReact", "Tag for the Enemy Hit React Ability")
		}
	}

	namespace Events
	{
		namespace Player
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "GPTags.Events.Player.Primary", "Tag for the Player Gameplay Primary Ability")
		}

		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "GPTags.Events.Enemy.HitReact", "Tag for the Enemy HitReact Enemy")
		}
	}
}

// C++ 코드에서 태그를 생성하는 이유
// C++ 코드에서 참조가 필요 없는 경우 언리얼 에디터를 통해 생성하면 되지만
// C++ 코드를 통해 참조가 이루어질 경우 C++ 코드에서 태그를 생성해야 접근할 수 있다.