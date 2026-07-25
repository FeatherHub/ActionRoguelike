#include "RogueCoinTestActor.h"

#include "Pickup/RogueCoinPickupSubsystem.h"

ARogueCoinTestActor::ARogueCoinTestActor()
{
	DefaultSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComp"));
	RootComponent = DefaultSceneComp;
	
	CoinSpawnRadius = 1024.f;
}

void ARogueCoinTestActor::SpawnCoins(int32 CoinAmount)
{
	URogueCoinPickupSubsystem* CoinPickupSubsystem = GetWorld()->GetSubsystem<URogueCoinPickupSubsystem>();

	CoinPickupSubsystem->SpawnCoins(CoinAmount, GetActorLocation(), CoinSpawnRadius);
}
