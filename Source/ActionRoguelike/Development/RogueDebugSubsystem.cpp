#include "RogueDebugSubsystem.h"

#include "ActionRoguelike.h"
#include "RogueNetUtil.h"
#include "RogueTimeUtil.h"

void URogueDebugSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	DebugContextQueue.Reset();
	
	FWorldDelegates::OnWorldTickEnd.AddUObject(this, &ThisClass::FlushDebugContextQueue);

	FString RunningContextString = IsNetModeServer(GetWorld()->GetNetMode()) ? TEXT("[SERVER]") : TEXT("[CLIENT]");
	
	UE_LOG(LogGame, Log, TEXT("[%s] WorldSubsystem %s Initialized"), *RunningContextString, TEXT(__FILE__));
}

void URogueDebugSubsystem::Submit(FScreenDebugContext Context)
{
	DebugContextQueue.Add(MoveTemp(Context));
}

void URogueDebugSubsystem::FlushDebugContextQueue(UWorld* World, ELevelTick LevelTick, float Delta)
{
	if(DebugContextQueue.IsEmpty())
	{
		return;
	}
	
	if(World != GetWorld())
	{
		return;
	}
	
	for (const FScreenDebugContext& DebugContext : DebugContextQueue)
	{
		FString DateTimeStamp = FString::Printf(TEXT("[%s] "), *RogueTimeUtil::ToMMSSMMM(DebugContext.DateTime)); 
		
		GEngine->AddOnScreenDebugMessage(
			DebugContext.DebugKey,
			0.f,
			DebugContext.Color,
			DateTimeStamp + DebugContext.Message);
	}

	bool bIsServer = IsNetModeServer(World->GetNetMode());
	GEngine->AddOnScreenDebugMessage(
		bIsServer ? 9999 : 8888,
		0.f,
		bIsServer ? FColor::Green : FColor::Blue,
		bIsServer ? TEXT("[SERVER]") : TEXT("[CLIENT]")
	);
	
	// update Remaining time
	for (FScreenDebugContext& DebugContext : DebugContextQueue)
	{
		DebugContext.RemainingTime -= Delta;
	}
	
	// remove expired DebugContexts
	for (int i = DebugContextQueue.Num() - 1; i >= 0; --i)
	{
		float RemainingTime = DebugContextQueue[i].RemainingTime;
		if(RemainingTime < 0.f)
		{
			DebugContextQueue.RemoveAt(i);
		}
	}
}