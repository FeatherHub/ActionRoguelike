#include "RogueAIController.h"

#include "BrainComponent.h"
#include "RogueAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"


void ARogueAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if(ARogueAICharacter* AICharacter = Cast<ARogueAICharacter>(InPawn))
	{
		AICharacter->OnDeathStart.AddDynamic(this, &ThisClass::OnPawnDeathStart);
	}
}

void ARogueAIController::BeginPlay()
{
	Super::BeginPlay();
	
	RunBehaviorTree(BehaviorTree);
	
	UBlackboardComponent* BB = GetBlackboardComponent();
	
	AActor* PlayerActor =  UGameplayStatics::GetPlayerPawn(this, 0);
	
	FName PlayerActorKeyName = TEXT("PlayerActor");
	BB->SetValueAsObject(PlayerActorKeyName, PlayerActor);
}

void ARogueAIController::OnPawnDeathStart()
{
	if(BrainComponent)
	{
		BrainComponent->StopLogic(TEXT("OnPawnDeathStart"));
	}
}
