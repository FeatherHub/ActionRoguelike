#include "RogueLibrary.h"

namespace RogueLibrary
{
	FString ToMMSSMMM(const FDateTime& DateTime)
	{
		return FString::Printf(TEXT("%02d:%02d.%03d"),
			DateTime.GetMinute(), DateTime.GetSecond(), DateTime.GetMillisecond());
	}

	void ApplyScalarParamter(UMaterialInstanceDynamic* MID, FName ParamName, float NewValue, float& OutLastValue)
	{
		if(FMath::IsNearlyEqual(NewValue, OutLastValue))
		{
			return;
		}

		MID->SetScalarParameterValue(ParamName, NewValue);
		OutLastValue = NewValue;
	}
}
