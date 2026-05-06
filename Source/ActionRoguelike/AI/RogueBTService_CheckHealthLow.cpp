#include "RogueBTService_CheckHealthLow.h"

#include "AIController.h"
#include "ActionSystem/RogueActionSystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

URogueBTService_CheckHealthLow::URogueBTService_CheckHealthLow()
{
	HealthLowRatio = 0.3f;
}

void URogueBTService_CheckHealthLow::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	//
	// AAIController* AIController = OwnerComp.GetAIOwner();
	// UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
	// URogueActionSystemComponent* ActionSystemComp = AIController->GetPawn()->GetComponentByClass<URogueActionSystemComponent>();
	//
	// float CurrentHealth = ActionSystemComp->GetCurrentHealth();
	// float MaxHealth = ActionSystemComp->GetMaxHealth();
	//
	// bool bIsHealthLow = CurrentHealth < MaxHealth * HealthLowRatio;
	//
	// BBComp->SetValueAsBool(IsHealthLowKey.SelectedKeyName, bIsHealthLow);
}
