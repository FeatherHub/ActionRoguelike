#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RogueAttributeSet.generated.h"


USTRUCT()
struct FRogueAttribute
{
	GENERATED_BODY()
	
	float Base;
	float Modifier;
};


UCLASS()
class ACTIONROGUELIKE_API URogueAttributeSet : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void PostApplyChange() {}
};


UCLASS()
class ACTIONROGUELIKE_API URogueHealthAttributeSet : public URogueAttributeSet
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	FRogueAttribute Health;

	UPROPERTY()
	FRogueAttribute HealthMax;
	
public:
	URogueHealthAttributeSet();
	virtual void PostApplyChange() override;
};
