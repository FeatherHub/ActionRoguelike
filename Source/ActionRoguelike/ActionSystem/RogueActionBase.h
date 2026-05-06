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
	UPROPERTY(EditDefaultsOnly, Category=Action)
	FName ActionName;
	
protected:
	UFUNCTION(BlueprintCallable)
	URogueActionSystemComponent* GetOwningComponent() const;

	UFUNCTION(BlueprintCallable)
	ACharacter* GetOwningCharacter() const;
	
public:
	UFUNCTION(BlueprintNativeEvent, Category=Action)
	void StartAction();
	
	UFUNCTION(BlueprintNativeEvent, Category=Action)
	void StopAction();
	
	FName GetActionName() const { return ActionName; }
};
