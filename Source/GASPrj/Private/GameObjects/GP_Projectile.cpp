
#include "GameObjects/GP_Projectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Character/GP_PlayerCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayTags/GPTags.h"
#include "Kismet/GameplayStatics.h"


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

	AGP_PlayerCharacter* PlayerCharacter = Cast<AGP_PlayerCharacter>(OtherActor);
	if (PlayerCharacter == nullptr) return;
	if (!PlayerCharacter->IsAlive()) return;

	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
	if (!IsValid(ASC) || !HasAuthority()) return;

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffect, 1.f, ContextHandle);
	
	// 데미지 이벤트 값을 Gameplay Effect에 전송해서 Attribute를 변경하는 방식
	// 해당하는 Gameplay Effect가 SetByCaller를 통해 어떤 Tag에 대한 이벤트를 받을지 이벤트 대기
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GPTags::SetByCaller::Projectile, Damage);
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

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