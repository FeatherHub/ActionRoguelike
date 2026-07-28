#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RogueActionType.generated.h"

class URogueActionBase;

//////////////
// Start 관련
//////////////

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

//////////////
// Stop 관련
//////////////

UENUM()
enum class ERogueStopActionReason : uint8
{
	Input, // 플레이어가 키를 뗌
	BlockedByTag, // 액션 실행 중, ASC에 부여된 태그에 의해 중지됨 (e.g. 기절)
	GrantedActionRemoved, // 액션이 박탈됨
};

USTRUCT(BlueprintType)
struct FRogueStopActionCause
{
	GENERATED_BODY()
	
	UPROPERTY()
	ERogueStopActionReason Reason = ERogueStopActionReason::Input;
	
	UPROPERTY()
	FGameplayTagContainer CausingTags;
};

USTRUCT(BlueprintType)
struct FRogueStopActionInfo
{
	GENERATED_BODY()
	
	UPROPERTY()
	FGameplayTag ActionTag;
	
	UPROPERTY()
	TSubclassOf<URogueActionBase> ActionClass;
	
	UPROPERTY()
	FRogueStopActionCause Cause;
};
