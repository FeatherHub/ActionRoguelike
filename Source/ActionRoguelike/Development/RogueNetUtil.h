#pragma once

#include "CoreMinimal.h"
#include "RogueDebugUtil.h"
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
FNetDebugContext GetNetDebugContext(const UObject* Object);

FString GetNetDebugName(const UObject* Object);

void DebugNetOnScreen(uint64 DebugKey, const FString& Msg, const FNetDebugContext& Context, float Duration);

#define DEBUG_NET_ONSCREEN_CVAR(Msg, CVar) \
	do if(CVar.GetValueOnGameThread()) \
	{ \
		DEBUG_NET_ONSCREEN(Msg); \
	} while(false)

#define DEBUG_NET_ONSCREEN(Msg) DEBUG_NET_ONSCREEN_EX(Msg, 10.f, 0)

#define DEBUG_NET_ONSCREEN_EX(Msg, Duration, DebugSubkey) do \
	{ \
		FNetDebugContext Context = GetNetDebugContext(this); \
		uint64 Hash1 = HashCombine(GetTypeHash(this), GetTypeHash(DebugSubkey)); \
		uint64 Hash2 = HashCombine(DEBUG_KEY_NET(Context.bIsNetModeServer), Hash1); \
		FString UserMsg = Msg; \
		DebugNetOnScreen(Hash2, UserMsg, Context, Duration); \
	} while(false) 

#define DEBUG_NET_ONSCREEN_EX_CVAR(CVar, Msg, Duration, DebugSubkey) \
	do if(CVar.GetValueOnGameThread()) \
	{ \
		DEBUG_NET_ONSCREEN_EX(Msg, Duration, DebugSubkey); \
	} while(false)
