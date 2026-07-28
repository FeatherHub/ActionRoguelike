#include "RogueToastMessageSystem.h"

void URogueToastMessageSystem::PushMessage(const FRogueToastMessage& Message)
{
	OnToastMessagePushed.Broadcast(Message);
}
