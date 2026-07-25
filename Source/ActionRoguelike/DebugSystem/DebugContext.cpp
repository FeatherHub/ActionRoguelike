#include "DebugContext.h"

#include "Network/NetUtil.h"

/////////////////
// FNetContext
/////////////////

FNetContext FNetContext::Make(const AActor* Actor)
{
	check(Actor);
	
	FNetContext Context {
		Actor->GetLocalRole(),
		Actor->HasAuthority(),
		NetUtil::ResolveNetControlStatus(Actor)
	};

	return Context;
}

FString FNetContext::ToString() const
{
	return FString::Printf(
		TEXT("%s | %s | Control %s"),
		*NetUtil::GetNetRoleString(NetLocalRole),
		bHasAuthority ? TEXT("Auth") : TEXT("No Auth"),
		*NetUtil::GetNetControlStatusString(ControlStatus)
	);
}

///////////////////////
// FNetDebugContext
///////////////////////

FNetDebugContext::FNetDebugContext(TOptional<FNetContext> NetContext, int32 PIEIndex, ENetMode NetMode): 
	NetContext(MoveTemp(NetContext)), PIEIndex(PIEIndex), NetMode(NetMode)
{
	bIsNetModeServer = NetUtil::IsNetModeServer(NetMode); 
}


FNetDebugContext FNetDebugContext::Make(const UWorld* World, const TOptional<FNetContext>& NetContext)
{
	FNetDebugContext DebugContext {
		NetContext,
		UE::GetPlayInEditorID(),
		World ? World->GetNetMode() : NM_MAX,
	};

	return DebugContext;
}

FString FNetDebugContext::ToString() const
{
	FString WorldContextString = FString::Printf(TEXT("PIE: %d | NetMode: %s"), PIEIndex, *NetUtil::GetNetModeString(NetMode));
	FString ActorContextString = NetContext ? NetContext.GetValue().ToString() : TEXT("[Net Context Not Available]");
		
	return WorldContextString + TEXT(" || ") + ActorContextString;
}
