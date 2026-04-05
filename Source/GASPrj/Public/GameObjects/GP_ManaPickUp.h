// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GP_ManaPickUp.generated.h"


class USceneComponent;
class USphereComponent;
class UGameplayEffect;

UCLASS()
class GASPRJ_API AGP_ManaPickUp : public AActor
{
	GENERATED_BODY()

public:
	AGP_ManaPickUp();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;

private:
	void SpawnOverlapParticle();
	
	UPROPERTY(VisibleAnywhere, Category = "GP|Component")
	TObjectPtr<USceneComponent> RootComp;
	UPROPERTY(VisibleAnywhere, Category = "GP|Component")
	TObjectPtr<USphereComponent> SphereComp;

	UPROPERTY(EditDefaultsOnly, Category = "GP|Mana")
	TSubclassOf<UGameplayEffect> ManaEffect;

	UPROPERTY(EditDefaultsOnly, Category = "GP|Particle")
	UParticleSystem* OverlapParticle;
};
