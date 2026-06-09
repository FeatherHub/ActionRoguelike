#include "RoguePlayerState.h"

#include "RogueSaveGame.h"


void ARoguePlayerState::SaveGame_Implementation(URogueSaveGame* SaveGameObject)
{
	if(SaveGameObject)
	{
		SaveGameObject->Credit = Credit;
	}
}

void ARoguePlayerState::Load_Implementation(URogueSaveGame* SaveGameObject)
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
