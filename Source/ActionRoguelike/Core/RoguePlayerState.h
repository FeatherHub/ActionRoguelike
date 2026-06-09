#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RoguePlayerState.generated.h"

class URogueSaveGame;

UCLASS()
class ACTIONROGUELIKE_API ARoguePlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
	int32 Credit;

public:
	UFUNCTION(BlueprintPure)
	int32 GetCredit() const { return Credit; }
	
	UFUNCTION(BlueprintCallable)
	void AddCredit(int32 Delta);
	
public:
	UFUNCTION(BlueprintNativeEvent)
	void WriteToSaveGame(URogueSaveGame* SaveGameObject);
	
	UFUNCTION(BlueprintNativeEvent)
	void LoadFromSaveGame(URogueSaveGame* SaveGameObject);
};
