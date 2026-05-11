#include "RogueCoinPickupSubsystem.h"

#include "NavigationSystem.h"

void URogueCoinPickupSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int i = 0; i < CoinLocations.Num(); ++i)
	{
		DrawDebugPoint(GetWorld(), CoinLocations[i], 8.f, FColor::White);
	}
}

void URogueCoinPickupSubsystem::SpawnCoins(int32 CoinCount, const FVector& Location, int32 Radius)
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(this);
	
	TArray<FVector> NewCoinLocations;
	NewCoinLocations.Reserve(CoinCount);
	TArray<int32> NewCoinCredits;
	NewCoinCredits.Reserve(CoinCount);
	
	for (int i = 0; i < CoinCount; ++i)
	{
		FNavLocation NavLocation;
		NavSystem->GetRandomPointInNavigableRadius(Location, Radius, NavLocation);
		
		NewCoinLocations.Add(NavLocation.Location);
		NewCoinCredits.Add(32);
	}
	
	AddCoins(NewCoinLocations, NewCoinCredits);
}

void URogueCoinPickupSubsystem::AddCoins(const TArray<FVector>& NewCoinLocations, const TArray<int32>& NewCoinCredits)
{
	CoinLocations.Append(NewCoinLocations);
	CoinCredits.Append(NewCoinCredits);
}
