#pragma once

#include "CoreMinimal.h"

namespace RogueLibrary
{
	FString ToMMSSMMM(const FDateTime& DateTime);
	void ApplyScalarParamter(UMaterialInstanceDynamic* MID, FName ParamName, float NewValue, float& OutLastValue); 
};
