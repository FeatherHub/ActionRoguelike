#include "RoguePlayerState.h"

#include "SaveSystem/RogueSaveGame.h"


void ARoguePlayerState::WriteToSaveGame_Implementation(URogueSaveGame* SaveGameObject)
{
	if(SaveGameObject)
	{
		SaveGameObject->Credit = Credit;
	}
}

void ARoguePlayerState::LoadFromSaveGame_Implementation(URogueSaveGame* SaveGameObject)
{
	if(SaveGameObject)
	{
		Credit = SaveGameObject->Credit;
	}
}

void ARoguePlayerState::AddCredit(int32 Delta)
{
	Credit += Delta;
}
