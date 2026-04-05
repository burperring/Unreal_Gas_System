
#include "GameObjects/GP_Projectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Character/GP_PlayerCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayTags/GPTags.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/GP_AbilitySystemBlueprintLibrary.h"


AGP_Projectile::AGP_Projectile()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	MuzzleParticleSpawnComp = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleParticleSpawnComp"));
	MuzzleParticleSpawnComp->SetupAttachment(RootComponent);
	ImpactParticleSpawnComp = CreateDefaultSubobject<USceneComponent>(TEXT("ImpactParticleSpawnComp"));
	ImpactParticleSpawnComp->SetupAttachment(RootComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AGP_Projectile::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		MuzzleParticle,										// UParticleSystem*
		MuzzleParticleSpawnComp->GetComponentLocation(),	// FVector
		MuzzleParticleSpawnComp->GetComponentRotation(),	// FRotator
		true												// AutoDestroy
	);
}

void AGP_Projectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	AGP_PlayerCharacter* PC = Cast<AGP_PlayerCharacter>(OtherActor);
	if (PC == nullptr) return;
	if (!PC->IsAlive()) return;

	UAbilitySystemComponent* ASC = PC->GetAbilitySystemComponent();
	if (!IsValid(ASC) || !HasAuthority()) return;

	// Send Set By Caller Damage Event
	FGameplayEffectContextHandle ContextHandle{
		UGP_AbilitySystemBlueprintLibrary::SendSetByCallerEvent(ASC, DamageEffect, GPTags::SetByCaller::Projectile, Damage)};

	// Send Player Hit React
	FGameplayEventData Payload;
	Payload.Target = PC;
	Payload.ContextHandle = ContextHandle;
	Payload.Instigator = GetOwner();
	UGP_AbilitySystemBlueprintLibrary::SendPlayerHitReact(PC, Payload);

	SpawnImmpactEffect();
	Destroy();
}

void AGP_Projectile::SpawnImmpactEffect() const
{
	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		ImpactParticle,										// UParticleSystem*
		ImpactParticleSpawnComp->GetComponentLocation(),	// FVector
		ImpactParticleSpawnComp->GetComponentRotation(),	// FRotator
		true												// AutoDestroy
	);
}