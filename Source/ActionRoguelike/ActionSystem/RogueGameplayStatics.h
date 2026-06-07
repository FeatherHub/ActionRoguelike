#pragma once

#include "CoreMinimal.h"

class URogueActionEffect;

class ACTIONROGUELIKE_API FRogueGameplayStatics
{
public:
	static float ApplyPointDamage(AActor* DamagedActor, float BaseDamage, FVector const& HitFromDirection, FHitResult const& HitInfo, AController* EventInstigator, AActor* DamageCauser, 
		TSubclassOf<UDamageType> DamageTypeClass, TSubclassOf<URogueActionEffect> ActionEffectOnHit = nullptr);
};
