#pragma once

#include "CoreMinimal.h"
#include "Network/NetType.h"

struct FOnScreenDebugContext
{
public:
	FDateTime DateTime; 
	float RemainingTime;
	uint64 DebugKey;
	FString Message;
	FColor Color;
	
public:
	FOnScreenDebugContext(FDateTime DateTime, float RemainingTime, uint64 DebugKey, const FString& Message, FColor Color)
		: DateTime(DateTime),
		  RemainingTime(RemainingTime),
		  DebugKey(DebugKey),
		  Message(Message),
		  Color(Color)
	{
	}
};

struct FNetContext
{
public:
	ENetRole NetLocalRole;
	bool bHasAuthority;
	ENetControlStatus ControlStatus;
	
public:
	FNetContext(ENetRole NetLocalRole, bool bHasAuthority, ENetControlStatus ControlStatus)
		: NetLocalRole(NetLocalRole),
		  bHasAuthority(bHasAuthority),
		  ControlStatus(ControlStatus)
	{
	}
	static FNetContext Make(const AActor* Actor);
	FString ToString() const;
};

struct FNetDebugContext
{
public:
	TOptional<FNetContext> NetContext;
	int32 PlayInEditorID;
	ENetMode NetMode;
	
public:
	FNetDebugContext(TOptional<FNetContext> NetContext, int32 PlayInEditorID, ENetMode NetMode): 
		NetContext(MoveTemp(NetContext)), PlayInEditorID(PlayInEditorID), NetMode(NetMode)
	{
	}
	
	FString ToString() const;
	static FNetDebugContext Make(const UWorld* World, const TOptional<FNetContext>& NetContext = {});
};
