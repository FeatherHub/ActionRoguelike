#include "RogueDebugSubsystem.h"

#include "DebugContext.h"
#include "DebugUtil.h"
#include "Development/RogueLibrary.h"
#include "Network/NetUtil.h"
#include "Containers/HashTable.h"

void URogueDebugSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	DebugContextQueue.Reset();
	WorldTickEndDelegateHandle = FWorldDelegates::OnWorldTickEnd.AddUObject(this, &ThisClass::FlushDebugContextQueue);

	FString NetModeName = NetUtil::GetNetModeString(GetWorld()->GetNetMode());
	DEBUG_ONSCREEN(0, 3.f, FColor::Green, TEXT("URogueDebugSubsystem Initialized"));
}

void URogueDebugSubsystem::Deinitialize()
{
	DebugContextQueue.Reset();
	FWorldDelegates::OnWorldTickEnd.Remove(WorldTickEndDelegateHandle);

	Super::Deinitialize();
}

void URogueDebugSubsystem::Submit(const FOnScreenDebugContext& Context)
{
	DebugContextQueue.Add(Context);
}

void URogueDebugSubsystem::FlushDebugContextQueue(UWorld* World, ELevelTick LevelTick, float Delta)
{
	if(World != GetWorld())
	{
		return;
	}
	
	if(DebugContextQueue.IsEmpty())
	{
		return;
	}
	
	for (const FOnScreenDebugContext& DebugContext : DebugContextQueue)
	{
		FString DateTimeStamp = FString::Printf(TEXT("[%s] "), *RogueLibrary::ToMMSSMMM(DebugContext.DateTime)); 
		
		GEngine->AddOnScreenDebugMessage(
			DebugContext.DebugKey,
			0.f,
			DebugContext.Color,
			DateTimeStamp + DebugContext.Message);
	}

	uint64 BannerDebugKey = HashPlayInEditorID(UE::GetPlayInEditorID());
	FColor BannerColor = NetUtil::IsNetModeServer(World->GetNetMode()) ? FColor::Green : FColor::Blue; 
	FString BannerString = FString::Printf(TEXT("[%s]"), *NetUtil::GetNetModeString(World->GetNetMode()));
	GEngine->AddOnScreenDebugMessage(
		BannerDebugKey,
		0.f,
		BannerColor,
		BannerString
	);
	
	// update Remaining time
	for (FOnScreenDebugContext& DebugContext : DebugContextQueue)
	{
		DebugContext.RemainingTime -= Delta;
	}
	
	// remove expired DebugContexts
	for (int i = DebugContextQueue.Num() - 1; i >= 0; --i)
	{
		float RemainingTime = DebugContextQueue[i].RemainingTime;
		if(RemainingTime < 0.f)
		{
			DebugContextQueue.RemoveAt(i);
		}
	}
}

uint64 URogueDebugSubsystem::HashPlayInEditorID(int32 PlayInEditorID)
{
	constexpr uint32 GoldenGamma = 0x9E3779B9u;   // 2^32/φ. 0 fixed point 회피 + 인접 입력 분산
	const uint32 Input = static_cast<uint32>(PlayInEditorID) + GoldenGamma;

	
	// MurmurFinalize32 결과가 uint32 이므로 uint64로 zero-extended 되어
	// 상위 32bit가 항상 0 -> (uint64)-1 sentinel에 도달할 수 없다
	return static_cast<uint64>(MurmurFinalize32(Input));
}
