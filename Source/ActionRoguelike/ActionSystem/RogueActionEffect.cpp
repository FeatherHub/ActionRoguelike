#include "RogueActionEffect.h"

#include "RogueActionSystemComponent.h"
#include "Misc/DataValidation.h"


void URogueActionEffect::StartAction_Implementation()
{
	Super::StartAction_Implementation();
	
	if (EffectDuration > 0.f)
	{
		RestartExpirationTimer();
	}
	
	if( PeriodInterval > 0.f )
	{
		++StackCount;
		if(bExecutePeriodicEffectOnStart)
		{
			ExecutePeriodicEffect();
		}
		GetWorld()->GetTimerManager().SetTimer(PeriodTimerHandle, this, &ThisClass::OnEffectPeriodTick, PeriodInterval, true);
	}
}

void URogueActionEffect::OnEffectPeriodTick()
{
	// Allow future per-tick logic (e.g. PeriodCount tracking)
	
	ExecutePeriodicEffect();
}

void URogueActionEffect::OnEffectExpired()
{
	// Prevent missing the final periodic tick when expiration and period timing overlap
	float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(PeriodTimerHandle);
	if(PeriodInterval > 0.f && RemainingTime > 0.f && RemainingTime < KINDA_SMALL_NUMBER)
	{
		ExecutePeriodicEffect();
	}

	GetWorld()->GetTimerManager().ClearTimer(ExpirationTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(PeriodTimerHandle);

	StopAction();
}

void URogueActionEffect::RestartExpirationTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(ExpirationTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(ExpirationTimerHandle, this, &ThisClass::OnEffectExpired, EffectDuration);
}

void URogueActionEffect::IncrementStackCount()
{
	++StackCount;
	RestartExpirationTimer();
}

void URogueActionEffect::StopAction_Implementation()
{
	Super::StopAction_Implementation();
	
	StackCount = 0;
	URogueActionSystemComponent* ASC = GetOwningComponent();
	
	if(ensure(ASC))
	{
		ASC->RemoveAction(this);
	}
}

#if WITH_EDITOR
EDataValidationResult URogueActionEffect::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	if (EffectDuration <= 0.f && PeriodInterval <= 0.f)
	{
		Context.AddWarning(NSLOCTEXT("Validation", "PeriodAndDurationNotSet",
			"ActionEffect has no duration and no period — it will do nothing."));
		Result = EDataValidationResult::Valid;
	}
	
	if (PeriodInterval > EffectDuration)
	{
		Context.AddWarning(NSLOCTEXT("Validation", "PeriodExceedsDuration",
			"Period is greater than Duration — it will do nothing."));
		Result = EDataValidationResult::Valid;
	}
	return Result;
}
#endif

void URogueActionEffect::ExecutePeriodicEffect_Implementation() { /* Empty */ }
