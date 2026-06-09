#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RogueSaveComponent.generated.h"

struct FActorSaveData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveLoaded);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROGUELIKE_API URogueSaveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnSaveLoaded OnSaveLoaded;
	
	UFUNCTION(BlueprintCallable)
	FActorSaveData GetActorSaveData() const;
	
	UFUNCTION(BlueprintPure)
	FName GetSaveId() const;
	
	UFUNCTION(BlueprintCallable)
	void ApplyActorSaveData(const FActorSaveData& ActorSaveData);
};
