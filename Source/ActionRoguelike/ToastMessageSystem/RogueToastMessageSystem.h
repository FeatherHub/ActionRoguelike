#pragma once

#include "CoreMinimal.h"
#include "RogueToastMessage.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "RogueToastMessageSystem.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnToastMessagePushed, const FRogueToastMessage&);

UCLASS()
class ACTIONROGUELIKE_API URogueToastMessageSystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	FOnToastMessagePushed OnToastMessagePushed;
	
	UFUNCTION(BlueprintCallable, Category=Toast)
	void PushMessage(const FRogueToastMessage& Message);
};
