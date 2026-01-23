
#include "GP_BaseCharacter.h"

AGP_BaseCharacter::AGP_BaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Tick and refresh bone transforms whether rendered or not - for bone updates on a dedicated server
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}



