#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "RogueDeveloperSetting.generated.h"

class UStaticMesh;

UCLASS(Config=Game, DefaultConfig)
class ACTIONROGUELIKE_API URogueDeveloperSetting : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditDefaultsOnly, Category=Pickup)
	TSoftObjectPtr<UStaticMesh> CoinMeshSoftAsset;
	
public:
	virtual FName GetCategoryName() const override
	{
		return FApp::GetProjectName();
	}
};
