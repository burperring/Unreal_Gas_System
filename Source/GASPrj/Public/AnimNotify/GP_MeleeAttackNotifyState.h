// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GP_MeleeAttackNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class GASPRJ_API UGP_MeleeAttackNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

private:
	TArray<FHitResult> PerformSphereTrace(USkeletalMeshComponent* MeshComp) const;
	void SendEventsToActors(USkeletalMeshComponent* MeshComp, const TArray<FHitResult>& Hits) const;
	
	UPROPERTY(EditAnywhere, Category = "GP|Debugs")
	bool bDrawDebugs = true;
	
	UPROPERTY(EditAnywhere, Category = "GP|Socket")
	FName SocketName{"FX_Trail_01_R"};
	UPROPERTY(EditAnywhere, Category = "GP|Socket")
	float SocketExtenstionOffset{40.f};
	UPROPERTY(EditAnywhere, Category = "GP|Socket")
	float SphereTraceRadius{60.f};
};
