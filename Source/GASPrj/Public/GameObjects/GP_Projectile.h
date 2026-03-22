// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GP_Projectile.generated.h"

class UGameplayEffect;
class UProjectileMovementComponent;

UCLASS()
class GASPRJ_API AGP_Projectile : public AActor
{
	GENERATED_BODY()

public:
	AGP_Projectile();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	virtual void BeginPlay() override;

private:
	void SpawnImmpactEffect() const;
	
	UPROPERTY(VisibleAnywhere, Category = "GP|Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	UPROPERTY(VisibleAnywhere, Category = "GP|Projectile")
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(VisibleAnywhere, Category = "GP|Projectile")
	TObjectPtr<USceneComponent> MuzzleParticleSpawnComp;
	UPROPERTY(VisibleAnywhere, Category = "GP|Projectile")
	TObjectPtr<USceneComponent> ImpactParticleSpawnComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "GP|Damage")
	TSubclassOf<UGameplayEffect> DamageEffect;
	// Gameplay Effect에서 적용되는 Attribute 값 변경은 항상 Add, Divide, Multify, Override만 존재한다.
	// 값을 줄이기 위해서는 -(음수) 값으로 지정해야 값이 줄어든다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GP|Damage", meta = (ExposeOnSpawn, ClampMax = "0.0", AllowPrivateAccess = true))
	float Damage{-10.f};
	
	UPROPERTY(EditDefaultsOnly, Category = "GP|Particle")
	UParticleSystem* MuzzleParticle;
	UPROPERTY(EditDefaultsOnly, Category = "GP|Particle")
	UParticleSystem* ImpactParticle;
};
