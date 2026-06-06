#pragma once

#include "CoreMinimal.h"
#include "RogueActionBase.h"
#include "RogueActionEffect.generated.h"

UCLASS()
class ACTIONROGUELIKE_API URogueActionEffect : public URogueActionBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category=Effect, meta=(ClampMin=0))
	float EffectDuration = 0.f;

	UPROPERTY(EditDefaultsOnly, Category=Effect, meta=(ClampMin=0))
	float PeriodInterval = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category=Effect, meta=(EditCondition="PeriodInterval > 0"))
	bool bExecutePeriodicEffectOnStart = false;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	int32 StackCount = 0;
	
	FTimerHandle ExpirationTimerHandle;
	FTimerHandle PeriodTimerHandle;
	
	void OnEffectPeriodTick();
	void OnEffectExpired();
	void RestartExpirationTimer();
	
public:
	virtual void StartAction_Implementation() override;
	virtual void StopAction_Implementation() override;
	
	UFUNCTION(BlueprintNativeEvent)
	void ExecutePeriodicEffect();

	void IncrementStackCount();
	int32 GetStackCount() const { return StackCount; }
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
