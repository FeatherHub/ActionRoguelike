#include "RogueEQC_TargetActor.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void URogueEQC_TargetActor::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	APawn* AIPawn = Cast<APawn>(QueryInstance.Owner.Get()); 
	AAIController* AIController = Cast<AAIController>(AIPawn->GetController());
	if(!ensure(AIController))
	{
		return;
	}
	
	UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
	if(ensure(BBComp))
	{
		FName TargetActorName = TEXT("PlayerActor");
		AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(TargetActorName));
		if (TargetActor)
		{
			UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetActor);
		}
	}
}
