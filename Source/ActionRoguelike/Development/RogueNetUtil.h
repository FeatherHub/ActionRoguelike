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

void DebugNetOnScreen(uint64 DebugKey, const FString& Msg, const FNetDebugContext& Context, float Duration);

#define DEBUG_NET_ONSCREEN_CVAR(Msg, CVar) \
	if(CVar.GetValueOnGameThread()) \
	{ \
		DEBUG_NET_ONSCREEN(Msg); \
	}

#define DEBUG_NET_ONSCREEN(Msg) ROGUE_DEBUG_NET_ONSCREEN_IMPL(Msg, 2.f)

#define ROGUE_DEBUG_NET_ONSCREEN_IMPL(Msg, Duration) do \
	{ \
		FNetDebugContext Context = GetNetDebugContext(this); \
		uint64 HashedThis = GetTypeHash(GetNameSafe(this)); \
		uint64 DebugKey = HashCombine(DEBUG_KEY_NET(Context.bIsNetModeServer), HashedThis); \
		FString UserMsg = Msg; \
		DebugNetOnScreen(DebugKey, UserMsg, Context, Duration); \
	} while(false) 

