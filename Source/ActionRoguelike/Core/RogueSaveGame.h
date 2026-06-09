#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RogueSaveGame.generated.h"


USTRUCT()
struct FActorSaveData
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FName ActorName;
	
	UPROPERTY()
	FTransform ActorTransform;
	
	UPROPERTY()
	TArray<uint8> ByteArray;
};

UCLASS()
class ACTIONROGUELIKE_API URogueSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	int32 Credit;
	
	UPROPERTY()
	TArray<FActorSaveData> ActorSaveDataArray;
};
