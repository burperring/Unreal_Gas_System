
#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Burst.h"
#include "GP_PlayerBurstImpact.generated.h"


UCLASS()
class GASPRJ_API UGP_PlayerBurstImpact : public UGameplayCueNotify_Burst
{
	GENERATED_BODY()

public:
	UGP_PlayerBurstImpact();

protected:
	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override;
	
private:
	void SpawnParticle(const FGameplayCueParameters& Parameters) const;
};
