#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RogueActionType.generated.h"

class URogueActionBase;

UENUM()
enum class ERogueCanStartFailure : uint8
{
	None,
	Running,
	Cooldown,
	Blocked,
	Cost
};

USTRUCT(BlueprintType)
struct FRogueCanStartResult
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FGameplayTag ActionTag;

	UPROPERTY()
	TSubclassOf<URogueActionBase> ActionClass;
	
	UPROPERTY()
	ERogueCanStartFailure Failure = ERogueCanStartFailure::None;
	
	UPROPERTY()
	float CooldownRemaining = 0.f;
	
	UPROPERTY()
	FGameplayTagContainer BlockedByTags;	
	
	UPROPERTY()
	FGameplayTag CostAttribute;
	
	UPROPERTY()
	float CostRequired = 0.f;
	
	UPROPERTY()
	float CostCurrent = 0.f;
	
public:
	bool IsSuccess() const { return Failure == ERogueCanStartFailure::None; }
	float GetMissingCost() const { return FMath::Max(0.f, CostRequired - CostCurrent); }
	FString ToDebugString() const;
};
