#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RogueSaveGame.generated.h"

UCLASS()
class ACTIONROGUELIKE_API URogueSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY();
	int32 Credit;
};
