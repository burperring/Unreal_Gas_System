
#include "GameObjects/GP_ManaPickUp.h"

#include "AbilitySystemComponent.h"
#include "Character/GP_BaseCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"


AGP_ManaPickUp::AGP_ManaPickUp()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);
	
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComp->SetupAttachment(GetRootComponent());
	SphereComp->SetSphereRadius(64.f);
}

void AGP_ManaPickUp::BeginPlay()
{
	Super::BeginPlay();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
}

void AGP_ManaPickUp::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	AGP_BaseCharacter* BaseCharacter = Cast<AGP_BaseCharacter>(OtherActor);
	if (BaseCharacter == nullptr) return;

	UAbilitySystemComponent* ASC = BaseCharacter->GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ManaEffect, 1.f, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	SpawnOverlapParticle();
	Destroy();
}

void AGP_ManaPickUp::SpawnOverlapParticle()
{
	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		OverlapParticle,		// UParticleSystem*
		GetActorLocation(),		// FVector
		FRotator::ZeroRotator,	// FRotator
		true					// AutoDestroy
	);
}