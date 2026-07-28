#pragma once

#include "GameplayTagContainer.h"
#include "RogueToastMessage.generated.h"

USTRUCT(BlueprintType)
struct FRogueToastMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category=Toast)
	FGameplayTag ChannelTag;
	
	UPROPERTY(BlueprintReadWrite, Category=Toast)
	FText Body;

	UPROPERTY(BlueprintReadWrite, Category=Toast)
	FGameplayTag SoundTag;
	
	UPROPERTY(BlueprintReadWrite, Category=Toast)
	float HoldTimeOverride = 0.f;
};
