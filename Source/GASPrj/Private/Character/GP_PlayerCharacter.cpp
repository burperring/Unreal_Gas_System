
#include "GASPrj/Public/Character/GP_PlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/GP_AttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GASPrj/Public/Player/GP_PlayerState.h"


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

UAttributeSet* AGP_PlayerCharacter::GetAttributeSet() const
{
	AGP_PlayerState* GPPlayerState = Cast<AGP_PlayerState>(GetPlayerState());
	if (GPPlayerState == nullptr) return nullptr;

	return GPPlayerState->GetAttributeSet();
}

void AGP_PlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC == nullptr || !HasAuthority()) return;

	// GAS System을 가져올 때 InitAbilityActorInfo를 사용하는 이유
	// Ability System Component(ASC)가 자신이 누구를 위해 동작해야 하는지 알게 해주기 때문
	// 만약 InitAbilityActorInfo를 호출하지 않을 경우 Ability Active 시 크래시, GameplayEffect 적용 실패, Attribute 접근 실패가 발생할 수 있다.
	// 즉 InitAbilityActorInfo는 Ability System Component(ASC)가 "누가 능력을 사용하는 지" 알 수 있도록 만드는 필수 초기화 단계이다.
	
	ASC->InitAbilityActorInfo(GetPlayerState(), this);
	OnASCInitialized.Broadcast(ASC, GetAttributeSet());
	
	GiveStartupAbilities();
	InitializeAttributes();
	
	SetAttributeValueChange();
}

void AGP_PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;

	ASC->InitAbilityActorInfo(GetPlayerState(), this);
	OnASCInitialized.Broadcast(ASC, GetAttributeSet());

	// 체력 변경에 따른 사망 이벤트 서버 동기화 작업 진행
	SetAttributeValueChange();
}
