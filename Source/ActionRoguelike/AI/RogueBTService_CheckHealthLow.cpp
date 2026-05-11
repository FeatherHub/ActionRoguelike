#include "RogueBTService_CheckHealthLow.h"

#include "AIController.h"
#include "ActionSystem/RogueActionSystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Core/RogueGameplayTag.h"

URogueBTService_CheckHealthLow::URogueBTService_CheckHealthLow()
{
	HealthLowRatio = 0.3f;
}

void URogueBTService_CheckHealthLow::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
	URogueActionSystemComponent* ASC = AIController->GetPawn()->GetComponentByClass<URogueActionSystemComponent>();
	
	float CurrentHealth = ASC->GetAttributeValue(RogueGameplayTag::Attribute_Health);
	float MaxHealth = ASC->GetAttributeValue(RogueGameplayTag::Attribute_HealthMax);
	
	bool bIsHealthLow = CurrentHealth < MaxHealth * HealthLowRatio;
	
	BBComp->SetValueAsBool(IsHealthLowKey.SelectedKeyName, bIsHealthLow);
}
