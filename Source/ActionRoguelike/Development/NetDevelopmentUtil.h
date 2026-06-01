#pragma once

#include "CoreMinimal.h"
#include "UObject/ReflectedTypeAccessors.h"


static bool IsServer(ENetMode NetMode)
{
	return (NetMode == NM_ListenServer) || (NetMode == NM_DedicatedServer);
}

static FString GetNetModeName(ENetMode NetMode)
{
	return IsServer(NetMode) ? "Server" : "Client";
}

inline FString GetNetDebugPrefix(APlayerController* PC, FColor* OutColor = nullptr)
{
	ENetMode NetMode = PC->GetWorld()->GetNetMode();
	
	if (OutColor)
	{
		*OutColor = IsServer(NetMode) ? FColor::Green : FColor::Blue;
	}
	
	return FString::Printf(
		TEXT("[PIE:%d | %s | %s | Auth:%d | LocalCtrl:%d]"),
		UE::GetPlayInEditorID(),
		*GetNetModeName(NetMode),
		*StaticEnum<ENetRole>()->GetNameStringByValue(PC->GetLocalRole()),
		PC->HasAuthority()        ? 1 : 0,
		PC->IsLocalController() ? 1 : 0
	);
}

inline FString GetNetDebugPrefix(APawn* Pawn, FColor* OutColor = nullptr)
{
	ENetMode NetMode = Pawn->GetWorld()->GetNetMode();
	
	if (OutColor)
	{
		*OutColor = IsServer(NetMode) ? FColor::Green : FColor::Blue;
	}
	
	return FString::Printf(
		TEXT("[PIE:%d | %s | %s | Auth:%d | LocalCtrl:%d]"),
		UE::GetPlayInEditorID(),
		*GetNetModeName(Pawn->GetWorld()->GetNetMode()),
		*StaticEnum<ENetRole>()->GetNameStringByValue(Pawn->GetLocalRole()),
		Pawn->HasAuthority()        ? 1 : 0,
		Pawn->IsLocallyControlled() ? 1 : 0
	);
}

#define ROGUE_SCREEN_DEBUG_NET(Message) ROGUE_SCREEN_DEBUG_NET_TIME(Message, 2.f)

#define ROGUE_SCREEN_DEBUG_NET_TIME(Message, TimeToDisplay) do \
	{ \
		FColor Color; \
		FString NetPrefix; \
		\
		APlayerController* PC = Cast<APlayerController>(GetOwner()); \
		if(PC) { NetPrefix = GetNetDebugPrefix(PC, &Color); } \
		APawn* Pawn = Cast<APawn>(GetOwner()); \
		if(Pawn) { NetPrefix = GetNetDebugPrefix(Pawn, &Color); } \
		GEngine->AddOnScreenDebugMessage(DEBUG_KEY, TimeToDisplay, Color, NetPrefix + Message  ); \
	} while(false) 