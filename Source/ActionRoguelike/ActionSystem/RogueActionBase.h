#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RogueActionBase.generated.h"

class URogueActionSystemComponent;
class ACharacter;

UCLASS(Abstract, Blueprintable)
class ACTIONROGUELIKE_API URogueActionBase : public UObject
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Action")
	FName ActionName;
	
protected:
	URogueActionSystemComponent* GetOwningComponent() const;
	ACharacter* GetOwningCharacter() const;
	
public:
	virtual void StartAction();
	FName GetActionName() const { return ActionName; }
};
