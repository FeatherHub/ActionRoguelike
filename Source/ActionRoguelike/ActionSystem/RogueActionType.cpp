#include "RogueActionType.h"

FString FRogueCanStartResult::ToDebugString() const
{
	if (Failure != ERogueCanStartFailure::None)
	{
		FString Title = FString::Printf(TEXT("Cannot Start Action %s. "), *ActionName.GetTagName().ToString());
		FString Reason = TEXT("Because of some reason");
		
		if(Failure == ERogueCanStartFailure::Running)
		{
			Reason = TEXT("Because it is running");
		}
		
		if(Failure == ERogueCanStartFailure::Cooldown)
		{
			Reason = FString::Printf(TEXT("Because Cooldown remains: %f"), CooldownRemaining);
		}
		
		if(Failure == ERogueCanStartFailure::Blocked)
		{
			Reason = FString::Printf(TEXT("Because it is blocked by %s"), *BlockedByTags.ToString());
		}
		
		if(Failure == ERogueCanStartFailure::Cost)
		{
			Reason = FString::Printf(TEXT("Because it needs %s %f. Current: %f"), 
				*CostAttribute.GetTagName().ToString(), CostRequired, CostCurrent);
		}
		
		return Title + Reason;
	}
	
	return FString::Printf(TEXT("Can Start Action %s"), *ActionName.ToString()); 
}
