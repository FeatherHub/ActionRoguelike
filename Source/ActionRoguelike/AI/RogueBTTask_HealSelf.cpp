#include "RogueBTTask_HealSelf.h"

#include "AIController.h"
#include "ActionSystem/RogueActionSystemComponent.h"
#include "Core/RogueGameplayTag.h"

EBTNodeResult::Type URogueBTTask_HealSelf::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();

	URogueActionSystemComponent* ASC = AIPawn->GetComponentByClass<URogueActionSystemComponent>();

	ASC->ApplyAttributeChange(RogueGameplayTag::Attribute_Health, HealAmount, BaseDelta);

	return EBTNodeResult::Succeeded;
}
