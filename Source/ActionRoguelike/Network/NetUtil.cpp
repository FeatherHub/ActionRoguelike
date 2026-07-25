#include "NetUtil.h"

#include "Engine/PackageMapClient.h"

namespace NetUtil
{
	ENetControlStatus ResolveNetControlStatus(const AActor* Actor)
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

	FString GetNetControlStatusString(ENetControlStatus NetControlStatus)
	{
		switch (NetControlStatus)
		{
		case ENetControlStatus::Local:
			return TEXT("Locally Controlled");
		case ENetControlStatus::NotLocal:
			return TEXT("Not Locally Controlled");
		case ENetControlStatus::NA:
			return TEXT("Not Available");
		}
		
		return TEXT("UNKNOWN");
	}

	bool IsNetModeServer(ENetMode NetMode)
	{
		return (NetMode == NM_ListenServer) || (NetMode == NM_DedicatedServer);
	}

	FString GetNetModeString(ENetMode NetMode)
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
		case NM_MAX:
			return TEXT("NA");
		}

		return TEXT("UNKNOWN");
	}

	FString GetNetRoleString(ENetRole NetRole)
	{
		switch (NetRole) {
		case ROLE_SimulatedProxy:
			return TEXT("SimulatedProxy");
		case ROLE_AutonomousProxy:
			return TEXT("AutonomousProxy");
		case ROLE_Authority:
			return TEXT("Authority");
		case ROLE_MAX:
		case ROLE_None:
			return TEXT("None");
		}
		
		return TEXT("UNKNOWN");
	}

	FString GetNetName(const UObject* Object)
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
}
