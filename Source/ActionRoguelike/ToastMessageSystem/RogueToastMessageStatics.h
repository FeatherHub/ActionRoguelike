#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RogueToastMessageStatics.generated.h"

struct FRogueToastMessage;

UCLASS()
class ACTIONROGUELIKE_API URogueToastMessageStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category=Toast)
	static void PushMessage(APlayerController* PC, const FRogueToastMessage& Message);
};
