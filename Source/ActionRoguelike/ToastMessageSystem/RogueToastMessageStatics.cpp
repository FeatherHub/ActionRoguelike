#include "RogueToastMessageStatics.h"

#include "RogueToastMessageSubsystem.h"

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

	URogueToastMessageSubsystem* RogueToastMessageSystem = ULocalPlayer::GetSubsystem<URogueToastMessageSubsystem>(LocalPlayer);
	if(!IsValid(RogueToastMessageSystem))
	{
		return;
	}
	
	RogueToastMessageSystem->PushMessage(Message);
}
