#include "RogueDebugSubsystem.h"

#include "DebugUtil.h"
#include "TimeUtil.h"

void URogueDebugSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	DebugContextQueue.Reset();
	
	FWorldDelegates::OnWorldTickEnd.AddUObject(this, &ThisClass::FlushDebugContextQueue);

	FString NetModeName = GetNetModeName(GetWorld()->GetNetMode());
	DEBUG_ONSCREEN_FMT(0, 3.f, FColor::Green, TEXT("[%s] WorldSubsystem %s Initialized"), *NetModeName, TEXT(__FILE__));
}

void URogueDebugSubsystem::Submit(const FOnScreenDebugContext& Context)
{
	DebugContextQueue.Add(Context);
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
	
	for (const FOnScreenDebugContext& DebugContext : DebugContextQueue)
	{
		FString DateTimeStamp = FString::Printf(TEXT("[%s] "), *TimeUtil::ToMMSSMMM(DebugContext.DateTime)); 
		
		GEngine->AddOnScreenDebugMessage(
			DebugContext.DebugKey,
			0.f,
			DebugContext.Color,
			DateTimeStamp + DebugContext.Message);
	}

	bool bIsServer = IsNetModeServer(World->GetNetMode());
	GEngine->AddOnScreenDebugMessage(
		UE::GetPlayInEditorID(),
		0.f,
		bIsServer ? FColor::Green : FColor::Blue,
		bIsServer ? TEXT("[SERVER]") : TEXT("[CLIENT]")
	);
	
	// update Remaining time
	for (FOnScreenDebugContext& DebugContext : DebugContextQueue)
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