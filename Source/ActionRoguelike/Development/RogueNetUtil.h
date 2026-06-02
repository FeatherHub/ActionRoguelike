#pragma once

#include "RogueNetUtil.generated.h"

static bool IsServer(ENetMode NetMode)
{
	return (NetMode == NM_ListenServer) || (NetMode == NM_DedicatedServer);
}

static FString GetNetModeName(ENetMode NetMode)
{
	return IsServer(NetMode) ? "Server" : "Client";
}

UENUM()
enum class ENetConrolStatus
{
	NA, Local, NotLocal
};

struct FNetContext
{
	int32 PIEIndex;
	ENetMode NetMode;
	ENetRole NetLocalRole;
	bool bHasAuthority;
	ENetConrolStatus ControlStatus;
	
	FString ToString();
};

inline ENetConrolStatus GetNetControlStatus(AActor* Actor)
{
	if (APlayerController* PC = Cast<APlayerController>(Actor))
	{
		return PC->IsLocalController() ? ENetConrolStatus::Local : ENetConrolStatus::NotLocal;
	}
	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		return Pawn->IsLocallyControlled() ? ENetConrolStatus::Local : ENetConrolStatus::NotLocal;
	}

	return ENetConrolStatus::NA;
}

inline FNetContext GetNetContext(AActor* Actor)
{
	FNetContext Context;
	Context.PIEIndex = UE::GetPlayInEditorID();
	Context.NetMode = Actor->GetWorld()->GetNetMode();
	Context.NetLocalRole = Actor->GetLocalRole();
	Context.bHasAuthority = Actor->HasAuthority();
	Context.ControlStatus = GetNetControlStatus(Actor);
	
	return Context;
}

struct FNetDebugContext
{
	FNetContext NetContext;
	FColor DebugColor;
	bool bIsServer;
};

inline FNetDebugContext GetNetDebugContext(UActorComponent* Comp)
{
	FNetContext NetContext = FNetContext{};

	if(AActor *Actor = Cast<AActor>(Comp->GetOwner()))
	{
		NetContext = GetNetContext(Actor);
	} 
	else
	{
		ensureMsgf(false, TEXT("Not implemented"));
	}
	
	bool bIsServer = IsServer(NetContext.NetMode);
	FColor DebugColor = bIsServer ? FColor::Green : FColor::Blue; 
	
	FNetDebugContext NetDebugContext { NetContext, DebugColor, bIsServer};
	return NetDebugContext;
}

#define SCREEN_DEBUG_NET(Msg) ROGUE_SCREEN_DEBUG_NET_TIME(Msg, 2.f)

#define ROGUE_SCREEN_DEBUG_NET_TIME(Msg, Duration) do \
	{ \
		FNetDebugContext Context = GetNetDebugContext(this); \
		uint64 DebugKey = DEBUG_KEY_NET(Context.bIsServer); \
		\
		GEngine->AddOnScreenDebugMessage( \
			DebugKey, \
			Duration, \
			Context.DebugColor, \
			Msg + " " + Context.NetContext.ToString() \
		); \
	} while(false) 

