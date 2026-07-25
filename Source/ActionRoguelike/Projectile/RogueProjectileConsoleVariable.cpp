#include "RogueProjectileConsoleVariable.h"

TAutoConsoleVariable<float> CVarProjectileDrawDebug{TEXT("rogue.projectile.DebugDraw"), 0.f, 
	TEXT("Draw projectile traces. (0=off. >0 is draw duration"), ECVF_Default};
