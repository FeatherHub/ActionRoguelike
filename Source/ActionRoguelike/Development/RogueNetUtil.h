#pragma once

#include "CoreMinimal.h"
#include "RogueNetUtil.generated.h"

UENUM()
enum class ENetControlStatus: uint8
{
	NA, Local, NotLocal
};

struct FNetContext
{
	ENetRole NetLocalRole;
	bool bHasAuthority;
	ENetControlStatus ControlStatus;
	
	FString ToString() const;
	
	static FNetContext Make(const AActor* Actor);
};

struct FNetDebugContext
{
	TOptional<FNetContext> NetContext;
	int32 PIEIndex;
	ENetMode NetMode;
	bool bIsNetModeServer;

	FColor GetDebugColor() const;
	FString ToString() const;
	static FNetDebugContext Make(const UWorld* World, TOptional<FNetContext> NetContext = {});
};

FNetDebugContext GetNetDebugContext(const AActor* Actor);
FNetDebugContext GetNetDebugContext(const UActorComponent* Comp);
bool ShouldSkipDebugMessage(const FNetDebugContext& Context);

#define SCREEN_DEBUG_NET(Msg) ROGUE_SCREEN_DEBUG_NET_DURATION(Msg, 2.f)

#define ROGUE_SCREEN_DEBUG_NET_DURATION(Msg, Duration) do \
	{ \
		FNetDebugContext DebugContext = GetNetDebugContext(this); \
		if(ShouldSkipDebugMessage(DebugContext)) { break; } \
		\
		uint64 DebugKey = DEBUG_KEY_NET(DebugContext.bIsNetModeServer); \
		FString DebugMsg = FString{Msg}; \
		\
		GEngine->AddOnScreenDebugMessage( \
			DebugKey, \
			Duration, \
			DebugContext.GetDebugColor(), \
			FString::Printf(TEXT("%s %s"), *DebugMsg, *DebugContext.ToString()) \
		); \
	} while(false) 

