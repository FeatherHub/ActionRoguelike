#include "RogueActionBase.h"

#include "ActionRoguelike.h"
#include "RogueActionSystemComponent.h"
#include "RogueAttributeSet.h"
#include "Development/RogueNetUtil.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

void URogueActionBase::StartAction_Implementation()
{
	ACharacter* Character = GetOwningCharacter();
	
	FString ActionMsg = FString::Printf(TEXT("[ActionBase::StartAction] Action(%s, %s). Character(%s)'s Controller(%s)"), 
		*ActionName.ToString(), *GetNetDebugName(this), *GetNetDebugName(Character), *GetNetDebugName(Character->GetController())); 
	
	ROGUE_DEBUG(0, ActionMsg, 3.f, FColor::Orange);
	
	bIsRunning = true;
	
	float CurrentTime = GetWorld()->TimeSeconds;
	CooldownEndTime = CurrentTime + CooldownTime;

	URogueActionSystemComponent* ASC = GetOwningComponent();
	ASC->AppendActiveTags(ActivationGrantTags);

	UE_LOGFMT(LogGame, Log, "Start Action '{ActionName}' at {GameTime}", ActionName.GetTagName(), CurrentTime);

	for (const TPair<FGameplayTag, float>& CostEntry : ActivationCostMap)
	{
		ASC->ApplyAttributeChange(CostEntry.Key, -CostEntry.Value, BaseDelta);
	}
}

void URogueActionBase::StopAction_Implementation()
{
	bIsRunning = false;
	
	URogueActionSystemComponent* ASC = GetOwningComponent();
	ASC->RemoveActiveTags(ActivationGrantTags);
	
	float GameTime = GetWorld()->TimeSeconds;
	UE_LOGFMT(LogGame, Log, "Stop Action '{ActionName}' at {GameTime}", ActionName.GetTagName(), GameTime);
}

bool URogueActionBase::CanStart() const
{
	if (IsRunning())
	{
		UE_LOGFMT(LogGame, Warning, "Cannot Start Action {ActionName}. Because it is in running", ActionName.GetTagName());
		return false;
	}
	
	float CooldownRemaining = GetCooldownRemaining();
	if (CooldownRemaining > 0.f)
	{
		UE_LOGFMT(LogGame, Warning, "Cannot Start Action {ActionName}. Because Cooldown remains: {CooldownRemaining}"
			, ActionName.GetTagName(), CooldownRemaining);
		return false;
	}

	URogueActionSystemComponent* ASC = GetOwningComponent();
	if (ASC->GetActiveTags().HasAny(ActivationBlockedTags))
	{
		UE_LOGFMT(LogGame, Warning, "Cannot Start Action {ActionName}. Because it blocks {BlockedTags}"
			, ActionName.GetTagName(), ActivationBlockedTags.ToString());
		return false;
	}

	for (const TPair<FGameplayTag, float>& CostEntry : ActivationCostMap)
	{
		if (FRogueAttribute* Attribute = GetOwningComponent()->GetAttribute(CostEntry.Key))
		{
			float CurrentAmount = Attribute->GetValue();
			if (CurrentAmount < CostEntry.Value)
			{
				UE_LOGFMT(LogGame, Warning
				, "Cannot Start Action {ActionName}. Because {CostName} is not enough. Needs: {NeedsAmount} Has: {HasAmount}"
				, ActionName.GetTagName(), *CostEntry.Key.ToString(), CostEntry.Value, CurrentAmount);
				
				return false;
			}
		}
	}
	
	return true;
}

bool URogueActionBase::CanStop() const
{
	if (!IsRunning())
	{
		UE_LOGFMT(LogGame, Error, "Cannot Stop Action {ActionName}. Becuase it is not in running.", ActionName.GetTagName());
		return false;	
	}
	
	return true;
}


float URogueActionBase::GetCooldownRemaining() const
{
	return FMath::Max(0.f, CooldownEndTime - GetWorld()->TimeSeconds);
}

URogueActionSystemComponent* URogueActionBase::GetOwningComponent() const
{
	return Cast<URogueActionSystemComponent>(GetOuter());
}

ACharacter* URogueActionBase::GetOwningCharacter() const
{
	return Cast<ACharacter>(GetOuter()->GetOuter());
}

void URogueActionBase::OnRep_IsRunning()
{
	if(bIsRunning)
	{
		StartAction();
	}
	else
	{
		StopAction();
	}
}

void URogueActionBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(URogueActionBase, bIsRunning);
}
