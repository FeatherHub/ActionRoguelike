#pragma once

#include "CoreMinimal.h"
#include "DebugUtil.generated.h"

UENUM()
enum class ENetControlStatus: uint8
{
	NA, // NotAvailable 
	Local, // LocallyControlled 
	NotLocal // RemotelyControlled
};

struct FOnScreenDebugContext
{
public:
	FDateTime DateTime; 
	float RemainingTime;
	uint64 DebugKey;
	FString Message;
	FColor Color;
	
public:
	FOnScreenDebugContext(FDateTime DateTime, float RemainingTime, uint64 DebugKey, const FString& Message, FColor Color)
		: DateTime(DateTime),
		  RemainingTime(RemainingTime),
		  DebugKey(DebugKey),
		  Message(Message),
		  Color(Color)
	{
	}
};

struct FNetContext
{
public:
	ENetRole NetLocalRole;
	bool bHasAuthority;
	ENetControlStatus ControlStatus;
	
public:
	FNetContext(ENetRole NetLocalRole, bool bHasAuthority, ENetControlStatus ControlStatus)
		: NetLocalRole(NetLocalRole),
		  bHasAuthority(bHasAuthority),
		  ControlStatus(ControlStatus)
	{
	}
	static FNetContext Make(const AActor* Actor);
	FString ToString() const;
};

struct FNetDebugContext
{
public:
	TOptional<FNetContext> NetContext;
	int32 PIEIndex;
	ENetMode NetMode;
	bool bIsNetModeServer;
	
public:
	FNetDebugContext(TOptional<FNetContext> NetContext, int32 PIEIndex, ENetMode NetMode);
	static FNetDebugContext Make(const UWorld* World, const TOptional<FNetContext>& NetContext = {});
	static FNetDebugContext Make(const AActor* Actor);
	static FNetDebugContext Make(const UActorComponent* Comp);
	static FNetDebugContext Make(const UObject* Object);
	static FNetDebugContext Make();

	FString ToString() const;
};

void SubmitDebugContext(UObject* WorldContext, uint64 DebugKey, const FString& Msg, const FColor& Color, float Duration, const FNetDebugContext& NetDebugContext);

bool IsNetModeServer(ENetMode NetMode);
FString GetNetDebugName(const UObject* Object);
FString GetNetModeName(ENetMode NetMode);

#define DEBUG_KEY_NET(IsServer) ((uint64)PointerHash( __FILE__, (__LINE__ * __LINE__ + IsServer)))

#if UE_BUILD_SHIPPING || UE_BUILD_TEST
	#define DEBUG_ONSCREEN(DebugSubkey, Duration, Color, Msg)
	#define DEBUG_ONSCREEN_FMT(DebugSubkey, Duration, Color, Fmt, ...)
	#define DEBUG_ONSCREEN_CVAR(CVar, DebugSubkey, Duration, Color, Msg)
	#define DEBUG_ONSCREEN_CVARFMT(CVar, DebugSubkey, Duration, Color, Fmt, ...)
#else
	#define DEBUG_ONSCREEN_CVARFMT(CVar, DebugSubkey, Duration, Color, Fmt, ...) \
		do if(CVar.GetValueOnGameThread()) { \
			DEBUG_ONSCREEN(DebugSubkey, Duration, Color, FString::Printf(Fmt, ##__VA_ARGS__)); \
		} while(false)

	#define DEBUG_ONSCREEN_CVAR(CVar, DebugSubkey, Duration, Color, Msg) \
		do if(CVar.GetValueOnGameThread()) { \
			DEBUG_ONSCREEN(DebugSubkey, Duration, Color, Msg); \
		} while(false)

	#define DEBUG_ONSCREEN_FMT(DebugSubkey, Duration, Color, Fmt, ...) \
		do { \
			DEBUG_ONSCREEN(DebugSubkey, Duration, Color, FString::Printf(Fmt, ##__VA_ARGS__)); \
		} while(false)

	#define DEBUG_ONSCREEN(DebugSubkey, Duration, Color, Msg) \
		do { \
			FNetDebugContext Context = FNetDebugContext::Make(this); \
			uint64 Hash1 = HashCombine(GetTypeHash(this), GetTypeHash(DebugSubkey)); \
 			uint64 Hash2 = HashCombine(DEBUG_KEY_NET(Context.bIsNetModeServer), Hash1); \
			SubmitDebugContext(this, Hash2, Msg, Color, Duration, Context); \
		} while(false)
#endif
