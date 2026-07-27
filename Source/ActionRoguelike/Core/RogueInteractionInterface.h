#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RogueInteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class URogueInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class ACTIONROGUELIKE_API IRogueInteractionInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void Interact(APawn* InstigatorPawn);
};
