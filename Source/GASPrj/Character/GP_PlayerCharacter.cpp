
#include "GP_PlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GASPrj/Player/GP_PlayerState.h"


AGP_PlayerCharacter::AGP_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

UAbilitySystemComponent* AGP_PlayerCharacter::GetAbilitySystemComponent() const
{
	AGP_PlayerState* GPPlayerState = Cast<AGP_PlayerState>(GetPlayerState());
	if (GPPlayerState == nullptr) return nullptr;

	return GPPlayerState->GetAbilitySystemComponent();
}

void AGP_PlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!IsValid(GetAbilitySystemComponent())) return;

	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
}

void AGP_PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (!IsValid(GetAbilitySystemComponent())) return;

	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
}
