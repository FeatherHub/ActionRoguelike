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

struct FScreenDebugContext
{
	double TimeStamp;  // internal purpose
	FDateTime DateTime; // display purpose
	float RemainingTime;

	uint64 DebugKey;
	FString Message;
	FColor Color;
	
	bool operator<(const FScreenDebugContext& Other) const 
	{
		return TimeStamp < Other.TimeStamp;
	}
};

bool IsNetModeServer(ENetMode NetMode);
FString GetNetDebugName(const UObject* Object);
FString GetDebugString(const FString& Msg, const FNetDebugContext& Context);

FNetDebugContext GetNetDebugContext(const AActor* Actor);
FNetDebugContext GetNetDebugContext(const UActorComponent* Comp);
FNetDebugContext GetNetDebugContext(const UObject* Object);

void SubmitDebugContext(UObject* WorldContext, uint64 DebugKey, const FString& Msg, const FColor& Color, float Duration, const FNetDebugContext& DebugContext);

#define ROGUE_DEBUG_CVARFMT(CVar, DebugSubkey, Duration, Color, Format, ...) \
	do if(CVar.GetValueOnGameThread()) \
	{ \
		ROGUE_DEBUG(DebugSubkey, FString::Printf(Format, ##__VA_ARGS__), Duration, Color); \
	} while(false);

#define ROGUE_DEBUG_CVAR(CVar, DebugSubkey, Msg, Duration, Color) \
	do if(CVar.GetValueOnGameThread()) { ROGUE_DEBUG(DebugSubkey, Msg, Duration, Color); } while(false);

#define ROGUE_DEBUG(DebugSubkey, Msg, Duration, Color) \
	do \
	{ \
		FNetDebugContext Context = GetNetDebugContext(this); \
		uint64 Hash1 = HashCombine(GetTypeHash(this), GetTypeHash(DebugSubkey)); \
 		uint64 Hash2 = HashCombine(DEBUG_KEY_NET(Context.bIsNetModeServer), Hash1); \
		SubmitDebugContext(this, Hash2, Msg, Color, Duration, Context); \
	} while(false);



// void DebugNetOnScreen(uint64 DebugKey, const FString& Msg, const FNetDebugContext& Context, float Duration);

// #define DEBUG_NET_ONSCREEN_CVAR(Msg, CVar) \
// 	do if(CVar.GetValueOnGameThread()) \
// 	{ \
// 		DEBUG_NET_ONSCREEN(Msg); \
// 	} while(false)
//
// #define DEBUG_NET_ONSCREEN(Msg) DEBUG_NET_ONSCREEN_EX(Msg, 10.f, 0)
//
// #define DEBUG_NET_ONSCREEN_EX(Msg, Duration, DebugSubkey) do \
// 	{ \
// 		FNetDebugContext Context = GetNetDebugContext(this); \
// 		uint64 Hash1 = HashCombine(GetTypeHash(this), GetTypeHash(DebugSubkey)); \
// 		uint64 Hash2 = HashCombine(DEBUG_KEY_NET(Context.bIsNetModeServer), Hash1); \
// 		FString UserMsg = Msg; \
// 		DebugNetOnScreen(Hash2, UserMsg, Context, Duration); \
// 	} while(false) 
//
// #define DEBUG_NET_ONSCREEN_EX_CVAR(CVar, Msg, Duration, DebugSubkey) \
// 	do if(CVar.GetValueOnGameThread()) \
// 	{ \
// 		DEBUG_NET_ONSCREEN_EX(Msg, Duration, DebugSubkey); \
// 	} while(false)
