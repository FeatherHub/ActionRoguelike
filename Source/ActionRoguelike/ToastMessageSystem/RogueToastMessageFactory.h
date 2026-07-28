#pragma once

#include "CoreMinimal.h"
#include "RogueToastMessage.h"

namespace RogueToastMessageFactory
{
	// Feedback — 내 입력에 대한 즉답
	FRogueToastMessage ActionCooldown(const FText& ActionName, int32 Seconds);
	FRogueToastMessage ActionBlocked(const FText& ActionName);
	FRogueToastMessage ActionCostShort(const FText& ActionName, int32 Missing);
	FRogueToastMessage ActionNotGranted(const FText& ActionName);

	// Notice — 알아두면 좋은 것
	FRogueToastMessage LevelUp(int32 NewLevel);
	FRogueToastMessage EnemySurge(int32 Count);

	// Headline — 하던 걸 멈추고 봐야 하는 것
	FRogueToastMessage BossAppear(const FText& BossName);
	FRogueToastMessage GameStart();
}
