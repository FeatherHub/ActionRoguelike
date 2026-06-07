#pragma once

#include "CoreMinimal.h"
#include "RogueProjectileBase.h"
#include "GameFramework/Actor.h"
#include "RogueProjectileMagic.generated.h"


class URogueActionEffect;

UCLASS(Abstract)
class ACTIONROGUELIKE_API ARogueProjectileMagic : public ARogueProjectileBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category=Damage)
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditDefaultsOnly, Category=Damage)
	float DamageAmount;
	
	UPROPERTY(EditDefaultsOnly, Category=Damage)
	TSubclassOf<URogueActionEffect> ActionEffectOnHit; 
	
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
public:
	ARogueProjectileMagic();
	virtual void PostInitializeComponents() override;
};
