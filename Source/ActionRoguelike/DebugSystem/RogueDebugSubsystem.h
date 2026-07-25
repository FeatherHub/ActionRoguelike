#pragma once

#include "CoreMinimal.h"
#include "DebugContext.h"
#include "Subsystems/WorldSubsystem.h"
#include "RogueDebugSubsystem.generated.h"

UCLASS()
class ACTIONROGUELIKE_API URogueDebugSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	void Submit(const FOnScreenDebugContext& Context);
	
protected:
	void FlushDebugContextQueue(UWorld* World, ELevelTick LevelTick, float Delta);
	
	TArray<FOnScreenDebugContext> DebugContextQueue;
};
