#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "RoguePickupSystemSetting.generated.h"

class UStaticMesh;
class USoundBase;

UCLASS(Config=Game, DefaultConfig)
class ACTIONROGUELIKE_API URoguePickupSystemSetting : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditDefaultsOnly, Category=Pickup, DisplayName="Coin Static Mesh")
	TSoftObjectPtr<UStaticMesh> CoinMeshSoftAsset;
	
	UPROPERTY(Config, EditDefaultsOnly, Category=Pickup, DisplayName="Coin Pickup Sound")
	TSoftObjectPtr<USoundBase> CoinPickupSoundSoftAsset;
	
	UPROPERTY(Config, EditDefaultsOnly, Category=Pickup)
	FName CoinPickupTriggerParameterName;
	
public:
	virtual FName GetCategoryName() const override
	{
		return FApp::GetProjectName();
	}
};
