#include "RogueAttributeSet.h"

URogueHealthAttributeSet::URogueHealthAttributeSet(): Health(100.f, 0.f), HealthMax(100.f, 0.f)
{
}

void URogueHealthAttributeSet::PostApplyChange()
{
	Super::PostApplyChange();
	
	Health.Base = FMath::Clamp(Health.Base, 0.f, HealthMax.Base);  
}
