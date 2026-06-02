#include "RogueNetUtil.h"

FString FNetContext::ToString()
{
	return FString::Printf(
		TEXT("[PIE:%d | %s | %s | Auth:%d | Control:%s]"),
		PIEIndex,
		*GetNetModeName(NetMode),
		*StaticEnum<ENetRole>()->GetNameStringByValue(NetLocalRole),
		bHasAuthority ? 1 : 0,
		*StaticEnum<ENetConrolStatus>()->GetNameStringByValue((int64)ControlStatus)
	);
}

