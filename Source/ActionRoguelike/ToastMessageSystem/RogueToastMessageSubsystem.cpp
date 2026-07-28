#include "RogueToastMessageSubsystem.h"

void URogueToastMessageSubsystem::PushMessage(const FRogueToastMessage& Message)
{
	OnToastMessagePushed.Broadcast(Message);
}
