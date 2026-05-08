#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RogueAttributeSet.generated.h"

class APawn;
class URogueActionSystemComponent;

USTRUCT()
struct FRogueAttribute
{
	GENERATED_BODY()
	
	float Base;
	float Modifier;
	float Multiplier;
	
	float GetValue() const
	{
		return (Base + Modifier) * Multiplier;
	}
};


UCLASS()
class ACTIONROGUELIKE_API URogueAttributeSet : public UObject
{
	GENERATED_BODY()
	
protected:
	URogueActionSystemComponent* GetOwningComponent() const;
	
public:
	virtual void PostInitializeComponents() {}
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

UCLASS()
class URogueCharacterAttributeSet : public URogueHealthAttributeSet
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	FRogueAttribute MoveSpeed;
	
public:
	virtual void PostInitializeComponents() override;
	void ApplyMoveSpeed();
	
protected:
	ACharacter* GetOwningCharacter() const;
	virtual void PostApplyChange() override;
};

UCLASS()
class URoguePlayerAttributeSet : public URogueCharacterAttributeSet
{
	GENERATED_BODY()

public:
	URoguePlayerAttributeSet();
};

UCLASS()
class URogueMonsterAttributeSet : public URogueCharacterAttributeSet
{
	GENERATED_BODY()

public:
	URogueMonsterAttributeSet();
};