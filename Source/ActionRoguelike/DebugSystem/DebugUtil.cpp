#include "DebugUtil.h"

#include "ActionRoguelike.h"
#include "DebugContext.h"
#include "RogueDebugSubsystem.h"
#include "Network/NetUtil.h"

static TAutoConsoleVariable<bool> CVarDebugShowNetContext{TEXT("rogue.debug.onscreen.ShowNetContext"), false,
	TEXT("Show net context for on-screen debug message. 1=Show, 0=Hide"), ECVF_Cheat};

static TAutoConsoleVariable<bool> CVarDebugToggleAll{TEXT("rogue.debug.onscreen.ToggleAll"), true,
    TEXT("Toggle all on-screen debug message. 1=ON, 0=OFF"), ECVF_Cheat};

static TAutoConsoleVariable<int32> CVarDebugPIEFilter{TEXT("rogue.debug.onscreen.PIEFilter"), -1,
    TEXT("Show messages from specific PIE Instance only. -1=Show All PIE Instances, or PIE ID"), ECVF_Cheat};

static TAutoConsoleVariable<int32> CVarDebugNetFilter{TEXT("rogue.debug.onscreen.NetModeFilter"), 0,
	TEXT("Show messages from specific NetMode Instance only. 0=Client and Server, 1=Client Only, 2=Server Only"), ECVF_Cheat};

// CVarDebugNetFilter의 옵션들과 짝을 맞춰야 합니다.
enum class ENetDebugFilter: int32
{
	ClientAndServer = 0,
	ClientOnly = 1,
	ServerOnly = 2,
};

static bool PassesPIEFilter()
{
	int32 PIEFilterOption = CVarDebugPIEFilter.GetValueOnGameThread();
	if(PIEFilterOption == -1)
	{
		return true;
	}
	
	return PIEFilterOption == UE::GetPlayInEditorID();
}

static bool PassesNetModeFilter(bool bIsNetModeServer)
{
	ENetDebugFilter DebugFilter = static_cast<ENetDebugFilter>(CVarDebugNetFilter.GetValueOnGameThread());

	switch (DebugFilter)
	{
	case ENetDebugFilter::ClientAndServer:
		return true;
	case ENetDebugFilter::ClientOnly:
		return !bIsNetModeServer;
	case ENetDebugFilter::ServerOnly:
		return bIsNetModeServer;
	}
	
	return false;
}

static FString GetDebugString(const FString& Msg, const FNetDebugContext& NetDebugContext)
{
	return CVarDebugShowNetContext.GetValueOnGameThread() 
		? FString::Printf(TEXT("%s %s"), *NetDebugContext.ToString().RightPad(100), *Msg)
		: FString::Printf(TEXT("%s"), *Msg);
}


static FNetDebugContext ResolveNetDebugContext(const UObject* Object)
{
	for(const UObject* Current = Object; Current; Current = Current->GetOuter())
	{
		if (const AActor* Actor = Cast<AActor>(Current))
		{
			return FNetDebugContext::Make(Actor->GetWorld(), FNetContext::Make(Actor));
		}
		
		if (const UActorComponent* Comp = Cast<UActorComponent>(Current))
		{
			if(const AActor* Owner = Comp->GetOwner())
			{
				return FNetDebugContext::Make(Owner->GetWorld(), FNetContext::Make(Owner));
			}
		}
	}

	return FNetDebugContext::Make(Object ? Object->GetWorld() : nullptr);
}

void SubmitDebugContext(UObject* ContextObject, uint64 DebugKey, const FString& Msg, const FColor& Color, float Duration)
{
	if(!CVarDebugToggleAll.GetValueOnGameThread())
	{
		return;
	}

	if(!PassesPIEFilter())
	{
		return;
	}
	
	if(!IsValid(ContextObject) || !IsValid(ContextObject->GetWorld()))
	{
		return;
	}

	UWorld* World = ContextObject->GetWorld();
	
	if(!PassesNetModeFilter(NetUtil::IsNetModeServer(World->GetNetMode())))
	{
		return;
	}
	
	URogueDebugSubsystem* DebugSubsystem = World->GetSubsystem<URogueDebugSubsystem>();
	
	if(!IsValid(DebugSubsystem))
	{
		UE_LOG(LogGame, Warning, TEXT("DebugSubSystem is not initialized yet"));
		return;
	}
	
	FNetDebugContext NetDebugContext = ResolveNetDebugContext(ContextObject);
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