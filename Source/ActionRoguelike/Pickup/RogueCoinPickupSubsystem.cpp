#include "RogueCoinPickupSubsystem.h"

#include "ActionRoguelike.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Player/RoguePlayerCharacter.h"

void URogueCoinPickupSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	// Temporary Hack
	FSoftObjectPath CoinMeshAssetPath = FSoftObjectPath("/Game/ExampleContent/Meshes/SM_Pickup_Coin.SM_Pickup_Coin");
	UStaticMesh* CoinMesh = Cast<UStaticMesh>(CoinMeshAssetPath.TryLoad());
	
	CoinISMComp = NewObject<UInstancedStaticMeshComponent>(GetWorld(), NAME_None, RF_Transient);
	CoinISMComp->RegisterComponentWithWorld(GetWorld());
	CoinISMComp->SetStaticMesh(CoinMesh);
	CoinISMComp->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}

void URogueCoinPickupSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector PlayerLocation = FVector::ZeroVector;
	float PickupRadius = 0.f;
	for (ARoguePlayerCharacter* PlayerCharacter : TActorRange<ARoguePlayerCharacter>(GetWorld()))
	{
		PlayerLocation = PlayerCharacter->GetActorLocation();
		PickupRadius = PlayerCharacter->GetPickupRadius();
	}


	TArray<int32> PickedCoinIndicies;
	for (int i = 0; i < CoinLocations.Num(); ++i)
	{
		float DistTo = FVector::Dist(PlayerLocation, CoinLocations[i]);
		if (DistTo < PickupRadius)
		{
			PickedCoinIndicies.Add(i);
		}
	}


	int32 TotalCoinCreditToGrant = 0.f;
	for (int i = PickedCoinIndicies.Num() - 1; i >= 0; --i)
	{
		int32 PickedCoinIndex = PickedCoinIndicies[i];
		
		TotalCoinCreditToGrant += CoinCredits[PickedCoinIndex];

		RemoveCoin(PickedCoinIndex);
	}
	
	UE_CLOG(TotalCoinCreditToGrant > 0, LogGame, Log, TEXT("Picked up Coin Total Credit: %d"), TotalCoinCreditToGrant)
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
		NewCoinCredits.Add(10);
	}
	

	TArray<FTransform> NewCoinTransforms;
	NewCoinTransforms.Reserve(CoinCount);
	
	for (int i = 0; i < CoinCount; ++i)
	{
		FTransform NewCoinTransform = FTransform(NewCoinLocations[i] + FVector{0.f, 0.f, 50.f} ); 
		NewCoinTransforms.Add(NewCoinTransform);
	}

	TArray<FPrimitiveInstanceId> NewCoinInstanceIds = CoinISMComp->AddInstancesById(NewCoinTransforms, true, false);
	
	AddCoins(NewCoinLocations, NewCoinCredits, NewCoinInstanceIds);
}

void URogueCoinPickupSubsystem::AddCoins(const TArray<FVector>& NewCoinLocations, const TArray<int32>& NewCoinCredits, const TArray<FPrimitiveInstanceId>& NewCoinInstanceIds)
{
	CoinLocations.Append(NewCoinLocations);
	CoinCredits.Append(NewCoinCredits);
	CoinMeshInstanceIds.Append(NewCoinInstanceIds);
}

void URogueCoinPickupSubsystem::RemoveCoin(int32 CoinIndex)
{
	CoinLocations.RemoveAt(CoinIndex);
	CoinCredits.RemoveAt(CoinIndex);
	
	CoinISMComp->RemoveInstanceById(CoinMeshInstanceIds[CoinIndex]);
	CoinMeshInstanceIds.RemoveAt(CoinIndex);
}
