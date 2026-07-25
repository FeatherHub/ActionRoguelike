#include "DebugUtil.h"

#include "ActionRoguelike.h"
#include "RogueDebugSubsystem.h"
#include "Engine/PackageMapClient.h"

static TAutoConsoleVariable<bool> CVarDebugToggleAll(TEXT("rogue.debug.onscreen.ToggleAll"), true,
	TEXT("Toggle all on-screen debug message. 1=ON, 0=OFF"), ECVF_Cheat);

static TAutoConsoleVariable<int> CVarDebugNetFilter{TEXT("rogue.debug.onscreen.NetFilter"), 0,
	TEXT("Filter net debug messages. 0=Client and Server, 1=Client Only, 2=Server Only"), ECVF_Cheat};

static TAutoConsoleVariable<bool> CVarDebugShowNetContext{TEXT("rogue.debug.onscreen.ShowNetContext"), false,
	TEXT("Show net context for on-screen debug message. 1=Show, 0=Hide"), ECVF_Cheat};

// CVarDebugNetFilter의 옵션들과 짝을 맞춰야 합니다.
enum class ENetDebugFilter: int 
{
	ClientAndServer = 0,
	ClientOnly = 1,
	ServerOnly = 2,
};

FString GetNetModeName(ENetMode NetMode);
ENetControlStatus GetNetControlStatus(const AActor* Actor);

/////////////////
// FNetContext
/////////////////

FNetContext FNetContext::Make(const AActor* Actor)
{
	check(Actor);
	
	FNetContext Context {
		Actor->GetLocalRole(),
		Actor->HasAuthority(),
		GetNetControlStatus(Actor)
	};
	
	return Context;
}

FString FNetContext::ToString() const
{
	return FString::Printf(
		TEXT("%s | %s | Control %s"),
		*StaticEnum<ENetRole>()->GetNameStringByValue(NetLocalRole),
		bHasAuthority ? TEXT("Auth") : TEXT("No Auth"),
		*StaticEnum<ENetControlStatus>()->GetNameStringByValue((int64)ControlStatus)
	);
}

///////////////////////
// FNetDebugContext
///////////////////////

FNetDebugContext::FNetDebugContext(TOptional<FNetContext> NetContext, int32 PIEIndex, ENetMode NetMode): 
	NetContext(MoveTemp(NetContext)), PIEIndex(PIEIndex), NetMode(NetMode)
{
	bIsNetModeServer = IsNetModeServer(NetMode); 
}

FNetDebugContext FNetDebugContext::Make()
{
	FNetDebugContext DebugContext {
		{},
		UE::GetPlayInEditorID(),
		NM_MAX,
	};

	return DebugContext;
}

FNetDebugContext FNetDebugContext::Make(const AActor* Actor)
{
	if(!Actor)
	{
		return Make();
	}
	
	return Make(Actor->GetWorld(),FNetContext::Make(Actor));
}

FNetDebugContext FNetDebugContext::Make(const UActorComponent* Comp)
{
	if(!Comp)
	{
		return Make();
	}
	
	if(AActor* Actor = Comp->GetOwner())
	{
		return Make(Actor);
	}
	
	return Make(Comp->GetWorld());
}

FNetDebugContext FNetDebugContext::Make(const UObject* Object)
{
	if(!Object)
	{
		return Make();
	}

	if(UActorComponent* ActorComp = Cast<UActorComponent>(Object->GetOuter()))
	{
		return Make(ActorComp);
	}

	if(AActor* Actor = Cast<AActor>(Object->GetOuter()))
	{
		return Make(Actor);
	}

	return Make(Object->GetWorld());
}

FNetDebugContext FNetDebugContext::Make(const UWorld* World, const TOptional<FNetContext>& NetContext)
{
	FNetDebugContext DebugContext {
		NetContext ,
		UE::GetPlayInEditorID(),
		World ? World->GetNetMode() : NM_MAX,
	};

	return DebugContext;
}

FString FNetDebugContext::ToString() const
{
	FString WorldContextString = FString::Printf(TEXT("PIE: %d | NetMode: %s"), PIEIndex, *GetNetModeName(NetMode));
	FString ActorContextString = NetContext ? NetContext.GetValue().ToString() : TEXT("[Net Context Not Available]");
		
	return WorldContextString + TEXT(" || ") + ActorContextString;
}

/////////////
// Util
/////////////

bool IsNetModeServer(ENetMode NetMode)
{
	return (NetMode == NM_ListenServer) || (NetMode == NM_DedicatedServer);
}

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


bool ShouldShowDebugMessage(const FNetDebugContext& NetDebugContext)
{
	ENetDebugFilter DebugFilter = static_cast<ENetDebugFilter>(CVarDebugNetFilter.GetValueOnGameThread());

	switch (DebugFilter)
	{
	case ENetDebugFilter::ClientAndServer:
		return true;
	case ENetDebugFilter::ClientOnly:
		return !NetDebugContext.bIsNetModeServer;
	case ENetDebugFilter::ServerOnly:
		return NetDebugContext.bIsNetModeServer;

	default:
		return false;
	}
}

FString GetDebugString(const FString& Msg, const FNetDebugContext& NetDebugContext)
{
	return CVarDebugShowNetContext.GetValueOnGameThread() 
		? FString::Printf(TEXT("%s %s"), *NetDebugContext.ToString().LeftPad(80), *Msg)
		: FString::Printf(TEXT("%s"), *Msg);
}

FString GetNetDebugName(const UObject* Object)
{
	if(!Object)
	{
		return FString{TEXT("None")};
	}
	
	if(UWorld* World = Object->GetWorld())
	{
		if(UNetDriver* NetDriver = World->GetNetDriver())
		{
			if(NetDriver->GuidCache.IsValid())
			{
				FNetworkGUID NetGUID = NetDriver->GuidCache->GetNetGUID(Object);
				if(NetGUID.IsValid())
				{
					return FString::Printf(TEXT("GUID%s"), *NetGUID.ToString()) ;
				}
			}
		}
	}
	
	return Object->GetFName().ToString();
}

void SubmitDebugContext(UObject* WorldContext, uint64 DebugKey, const FString& Msg, const FColor& Color, float Duration, const FNetDebugContext& NetDebugContext)
{
	if(!CVarDebugToggleAll.GetValueOnGameThread())
	{
		return;
	}
	
	if(!ShouldShowDebugMessage(NetDebugContext))
	{
		return;
	}
	
	if(!IsValid(WorldContext) || !IsValid(WorldContext->GetWorld()))
	{
		return;
	}
	
	UWorld* World = WorldContext->GetWorld();
	
	
	URogueDebugSubsystem* DebugSubsystem = World->GetSubsystem<URogueDebugSubsystem>();
	
	if(!IsValid(DebugSubsystem))
	{
		UE_LOG(LogGame, Warning, TEXT("DebugSubSystem is not initialized yet"));
		return;
	}
	
	FString FinalMsg = GetDebugString(Msg, NetDebugContext);
	
	FOnScreenDebugContext ScreenDebugContext {
		FDateTime::Now(),
		Duration,
		DebugKey,
		FinalMsg,
		Color,
	};
	
	DebugSubsystem->Submit(ScreenDebugContext);
}