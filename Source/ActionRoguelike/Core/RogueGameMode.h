#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameMode.h"
#include "RogueGameMode.generated.h"

class ARogueAICharacter;
class UEnvQueryInstanceBlueprintWrapper;

UCLASS()
class ACTIONROGUELIKE_API ARogueGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category=Spawn)
	TSubclassOf<ARogueAICharacter> BotClass;
	
	UPROPERTY(EditDefaultsOnly, Category=Spawn)
	TObjectPtr<UEnvQuery> EnvQuery_FindBotSpawnLocation;

	UPROPERTY(EditDefaultsOnly, Category=Spawn)
	TObjectPtr<UCurveFloat> SpawnBotMaxCurve;
	
	UPROPERTY(EditDefaultsOnly, Category=Spawn, meta=(ClampMin=0))
	float SpawnBotInterval;
	
	FTimerHandle SpawnBotTimer;
	
	UFUNCTION()
	void SpawnBot();
	
	UFUNCTION()
	void OnEnvQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
private:
	UPROPERTY()
	TObjectPtr<UEnvQueryInstanceBlueprintWrapper> EnvQueryInstance;
	
public:
	ARogueGameMode();
	virtual void StartPlay() override;
	
#if !UE_BUILD_SHIPPING
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
