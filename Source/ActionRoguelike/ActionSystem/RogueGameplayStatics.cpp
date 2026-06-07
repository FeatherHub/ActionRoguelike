#include "RogueGameplayStatics.h"

#include "ActionSystem/RogueActionSystemComponent.h"
#include "ActionSystem/RogueActionEffect.h"
#include "Kismet/GameplayStatics.h"


float FRogueGameplayStatics::ApplyPointDamage(AActor* DamagedActor, float BaseDamage, FVector const& HitFromDirection, FHitResult const& HitInfo, AController* EventInstigator, AActor* DamageCauser, 
                                             TSubclassOf<UDamageType> DamageTypeClass, TSubclassOf<URogueActionEffect> ActionEffectOnHit)
{
	float ActualDamage = UGameplayStatics::ApplyPointDamage(DamagedActor, BaseDamage, HitFromDirection, HitInfo, EventInstigator, DamageCauser, DamageTypeClass);
	
	if(ActionEffectOnHit)
	{
		if(URogueActionSystemComponent* ASC = DamagedActor->FindComponentByClass<URogueActionSystemComponent>())
		{
			ASC->GrantAction(ActionEffectOnHit);
		}
	}
	
	return ActualDamage;
}
