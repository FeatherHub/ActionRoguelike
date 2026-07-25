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
		TEXT("Role %s | %s | %s"),
		*NetUtil::GetNetRoleString(NetLocalRole),
		bHasAuthority ? TEXT("Auth") : TEXT("No Auth"),
		*NetUtil::GetNetControlStatusString(ControlStatus)
	);
}

///////////////////////
// FNetDebugContext
///////////////////////

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
	FString WorldContextString = FString::Printf(TEXT("PIE %d %s"), PlayInEditorID, *NetUtil::GetNetModeString(NetMode));
	FString ActorContextString = NetContext ? NetContext.GetValue().ToString() : TEXT("[No Net Context]");
		
	return WorldContextString + TEXT(" | ") + ActorContextString;
}
