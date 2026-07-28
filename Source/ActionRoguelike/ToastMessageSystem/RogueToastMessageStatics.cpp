#include "RogueToastMessageStatics.h"

#include "RogueToastMessageSystem.h"

void URogueToastMessageStatics::PushMessage(APlayerController* PC, const FRogueToastMessage& Message)
{
	if(!IsValid(PC))
	{
		return;
	}
	
	ULocalPlayer* LocalPlayer =  PC->GetLocalPlayer();
	if(!IsValid(LocalPlayer))
	{
		return;
	}

	URogueToastMessageSystem* RogueToastMessageSystem = ULocalPlayer::GetSubsystem<URogueToastMessageSystem>(LocalPlayer);
	if(!IsValid(RogueToastMessageSystem))
	{
		return;
	}
	
	RogueToastMessageSystem->PushMessage(Message);
}
