#include "RogueNetUtil.h"

static TAutoConsoleVariable<int> CVarNetDebugVisibility{TEXT("rogue.net.debug.Visibility"), 0,
	TEXT("Set net debug visibility. 0=Client and Server, 1=Client Only, 2=Server Only"), ECVF_Cheat};

enum class NetDebugVisibility
{
	ClientAndServer = 0,
	ClientOnly = 1,
	ServerOnly = 2,
};

bool IsNetModeServer(ENetMode NetMode);
FString GetNetModeName(ENetMode NetMode);
ENetControlStatus GetNetControlStatus(const AActor* Actor);

/////////////////
// FNetContext
/////////////////

FNetContext FNetContext::Make(const AActor* Actor)
{
	check(Actor);
	
	FNetContext Context;
	Context.NetLocalRole = Actor->GetLocalRole();
	Context.bHasAuthority = Actor->HasAuthority();
	Context.ControlStatus = GetNetControlStatus(Actor);
	
	return Context;
}

FString FNetContext::ToString() const
{
	return FString::Printf(
		TEXT("[%s | Auth: %d | Control: %s]"),
		*StaticEnum<ENetRole>()->GetNameStringByValue(NetLocalRole),
		bHasAuthority ? 1 : 0,
		*StaticEnum<ENetControlStatus>()->GetNameStringByValue((int64)ControlStatus)
	);
}

///////////////////////
// FNetDebugContext
///////////////////////

FNetDebugContext FNetDebugContext::Make(const UWorld* World, TOptional<FNetContext> NetContext)
{
	FNetDebugContext DebugContext;
	DebugContext.NetContext = MoveTemp(NetContext);
	DebugContext.NetMode = World ? World->GetNetMode() : NM_MAX;
	DebugContext.bIsNetModeServer = IsNetModeServer(DebugContext.NetMode);
	DebugContext.PIEIndex = UE::GetPlayInEditorID();
	
	return DebugContext;
}

FColor FNetDebugContext::GetDebugColor() const
{
	return bIsNetModeServer ? FColor::Green : FColor::Blue;
}

FString FNetDebugContext::ToString() const
{
	FString DebugContextString = FString::Printf(TEXT("[PIE: %d | NetMode: %s]"), PIEIndex, *GetNetModeName(NetMode));
	FString NetContextString = NetContext ? NetContext.GetValue().ToString() : TEXT("[Net Context Not Available]");
		
	return DebugContextString + NetContextString;
}

///////////////////
// Util
///////////////////

FString GetNetModeName(ENetMode NetMode)
{
	switch(NetMode)
	{
	case NM_Standalone:
		return TEXT("Standalone");
	case NM_DedicatedServer:
	case NM_ListenServer:
		return TEXT("Server");
	case NM_Client:
		return TEXT("Client");
	default:
		return TEXT("NA");
	}
}

bool IsNetModeServer(ENetMode NetMode)
{
	return (NetMode == NM_ListenServer) || (NetMode == NM_DedicatedServer);
}

ENetControlStatus GetNetControlStatus(const AActor* Actor)
{
	if (const APlayerController* PC = Cast<APlayerController>(Actor))
	{
		return PC->IsLocalController() ? ENetControlStatus::Local : ENetControlStatus::NotLocal;
	}
	if (const APawn* Pawn = Cast<APawn>(Actor))
	{
		return Pawn->IsLocallyControlled() ? ENetControlStatus::Local : ENetControlStatus::NotLocal;
	}

	return ENetControlStatus::NA;
}

FNetDebugContext GetNetDebugContext(const AActor* Actor)
{
	if(!Actor)
	{
		return FNetDebugContext::Make(nullptr);
	}
	
	return FNetDebugContext::Make(Actor->GetWorld(),FNetContext::Make(Actor));
}

FNetDebugContext GetNetDebugContext(const UActorComponent* Comp)
{
	if(!Comp)
	{
		return FNetDebugContext::Make(nullptr);
	}
	
	if(AActor* Actor = Comp->GetOwner())
	{
		return GetNetDebugContext(Actor);
	}
	
	return FNetDebugContext::Make(Comp->GetWorld());
}

bool ShouldSkipDebugMessage(const FNetDebugContext& Context)
{
	int DebugVisibility = CVarNetDebugVisibility.GetValueOnGameThread();
	
	if(DebugVisibility == (int) NetDebugVisibility::ServerOnly && !Context.bIsNetModeServer)
	{
		return true;
	}
	if(DebugVisibility == (int) NetDebugVisibility::ClientOnly && Context.bIsNetModeServer)
	{
		return true;
	}
	
	return false;
}
