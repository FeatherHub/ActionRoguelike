#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RogueCoinTestActor.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ARogueCoinTestActor : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, Category=Component)
	TObjectPtr<USceneComponent> DefaultSceneComp;
	
	UPROPERTY(EditAnywhere, Category=Coin)
	int32 CoinSpawnRadius;
	
public:
	ARogueCoinTestActor();
	
	UFUNCTION(BlueprintCallable)
	void SpawnCoins(int32 CoinAmount);
};
