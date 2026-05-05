#pragma once

#include "CoreMinimal.h"
#include "RogueActionBase.h"
#include "RogueAction_ProjectileAttack.generated.h"

class UNiagaraSystem;
class ARogueProjectileBase;
class USoundBase;
class UAnimMontage;

UCLASS(Abstract)
class ACTIONROGUELIKE_API URogueAction_ProjectileAttack : public URogueActionBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category=Attack)
	FName MuzzleSocketName;
		
	UPROPERTY(EditDefaultsOnly, Category=Attack)
	TSubclassOf<ARogueProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category=Attack)
	float AttackDelay;
	
	UPROPERTY(EditDefaultsOnly, Category=Attack)
	TObjectPtr<UAnimMontage> AnimMontage_Attack;

	UPROPERTY(EditDefaultsOnly, Category=Attack)
	TObjectPtr<UNiagaraSystem> CastingEffect;
	
	UPROPERTY(EditDefaultsOnly, Category=Attack)
	TObjectPtr<USoundBase> CastingSound;	
	
protected:
	void SpawnProjectile();
	
public:
	URogueAction_ProjectileAttack();
	virtual void StartAction() override;
};
