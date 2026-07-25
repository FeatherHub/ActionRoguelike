#pragma once

#include "CoreMinimal.h"
#include "NetType.h"

namespace NetUtil
{
	// ENetControlStatus
	ENetControlStatus ResolveNetControlStatus(const AActor* Actor);
	FString GetNetControlStatusString(ENetControlStatus NetControlStatus);
	
	// ENetMode
	bool IsNetModeServer(ENetMode NetMode);
	FString GetNetModeString(ENetMode NetMode);
	
	// ENetRole
	FString GetNetRoleString(ENetRole NetRole);
	
	// NetName
	FString GetNetName(const UObject* Object);
}
