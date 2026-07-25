#include "RogueAIConsoleVariable.h"

TAutoConsoleVariable<bool> CVarMinionRangedDrawDebug{TEXT("rogue.ai.minionranged.DebugDraw"), false,
	TEXT("Draw MinionRanged related shapes. (0=off, 1=on)"), ECVF_Default};
