#include "RoguePotionHealth.h"

#include "ActionSystem/RogueActionSystemComponent.h"
#include "Core/RogueGameplayTag.h"


ARoguePotionHealth::ARoguePotionHealth()
{
	HealthIncreaseAmount = 20.f;
}

void ARoguePotionHealth::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBeginOverlap(OverlappedComp, Other, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	URogueActionSystemComponent* ASC = Other->GetComponentByClass<URogueActionSystemComponent>();
	if (!ASC)
	{
		return;
	}


	
	if (ASC->ApplyAttributeChange(RogueGameplayTag::Attribute_Health, HealthIncreaseAmount, BaseDelta))
	{
		PlayPickupEffect();
		Destroy();		
	}
}
