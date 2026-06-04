#include "RogueTimeUtil.h"

FString RogueTimeUtil::ToMMSSMMM(const FDateTime& DateTime)
{
	return FString::Printf(TEXT("%02d:%02d.%03d"),
		DateTime.GetMinute(), DateTime.GetSecond(), DateTime.GetMillisecond());
}
