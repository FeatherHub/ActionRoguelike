#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RogueDebugSubsystem.generated.h"

struct FScreenDebugContext;

UCLASS()
class ACTIONROGUELIKE_API URogueDebugSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	void Submit(FScreenDebugContext Context);
	
protected:
	void FlushDebugContextQueue(UWorld* World, ELevelTick LevelTick, float Delta);
	
	TArray<FScreenDebugContext> DebugContextQueue;
};
