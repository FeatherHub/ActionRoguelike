#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RogueCoinPickupSubsystem.generated.h"

UCLASS()
class ACTIONROGUELIKE_API URogueCoinPickupSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
protected:
	TArray<FVector> CoinLocations;
	TArray<int32> CoinCredits;
	TArray<FPrimitiveInstanceId> CoinMeshInstanceIds;
	
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> CoinISMComp;

	UPROPERTY()
	TObjectPtr<UAudioComponent> CoinPickupAudioComp;
	
	// Cache Trigger Name from Setting for Audio Component
	FName CoinPickupTriggerName;
	
protected:
	void OnCoinMeshLoadComplete(const FSoftObjectPath& SoftObjectPath, UObject* LoadedAsset);
	void OnCoinPickupSoundLoadComplete(const FSoftObjectPath& SoftObjectPath, UObject* LoadedAsset);
	
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
	
	void SpawnCoins(int32 CoinCount, const FVector& Location, int32 Radius);
	void AddCoins(const TArray<FVector>& NewCoinLocations, const TArray<int32>& NewCoinCredits, const TArray<FPrimitiveInstanceId>& NewCoinInstanceIds);
	void RemoveCoin(int32 CoinIndex);

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(URogueCoinPickupSubsystem, STATGROUP_Tickables);
	}
};
