#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "RogueAIController.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ARogueAIController : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category=AI)
	TObjectPtr<UBehaviorTree> BehaviorTree;
	
	UFUNCTION()
	virtual void OnPawnDeathStart();
public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
};
