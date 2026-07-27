#include "RogueActionBase.h"

#include "ActionRoguelike.h"
#include "RogueActionSystemComponent.h"
#include "RogueAttributeSet.h"
#include "DebugSystem/DebugUtil.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Network/NetUtil.h"

void URogueActionBase::StartAction_Implementation()
{
	ACharacter* Character = GetOwningCharacter();
	
	DEBUG_ONSCREEN_FMT(0, 3.f, FColor::Orange, 
		TEXT("[ActionBase::StartAction] Action(%s, %s). Character(%s)'s Controller(%s)"),
		*ActionName.ToString(), *NetUtil::GetNetName(this), *NetUtil::GetNetName(Character), *NetUtil::GetNetName(Character->GetController()));
	
	
	if(CooldownStartPolicy == ECooldownStartPolicy::OnStart)
	{
		StartCooldown();
	}
	
	URogueActionSystemComponent* ASC = GetOwningComponent();
	ASC->AppendActiveTags(ActivationGrantTags);

	UE_LOGFMT(LogGame, Log, "Start Action '{ActionName}' at {GameTime}", ActionName.GetTagName(), GetWorld()->TimeSeconds);

	for (const TPair<FGameplayTag, float>& CostEntry : ActivationCostMap)
	{
		ASC->ApplyAttributeChange(CostEntry.Key, -CostEntry.Value, BaseDelta);
	}
	
	bIsRunning = true;
}


void URogueActionBase::StopAction_Implementation()
{
	bIsRunning = false;
	
	if(CooldownStartPolicy == ECooldownStartPolicy::OnStop)
	{
		StartCooldown();
	}
	
	URogueActionSystemComponent* ASC = GetOwningComponent();
	ASC->RemoveActiveTags(ActivationGrantTags);
	
	UE_LOGFMT(LogGame, Log, 
		"Stop Action '{ActionName}' at {GameTime}", ActionName.GetTagName(), GetWorld()->TimeSeconds);
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

/////////////
// Cooldown
/////////////

void URogueActionBase::StartCooldown()
{
	if(CooldownTime <= 0.f)
	{
		return;
	}
	
	CooldownEndTime = GetWorld()->TimeSeconds + CooldownTime;
}


float URogueActionBase::GetCooldownRemaining() const
{
	return FMath::Max(0.f, CooldownEndTime - GetWorld()->TimeSeconds);
}


float URogueActionBase::GetCooldownProgress() const
{
	if(CooldownTime <= 0.f)
	{
		return 0.f;
	}
	
	return FMath::Clamp(GetCooldownRemaining() / CooldownTime, 0.f, 1.f);
}

/////////////////////////////////
// 변수 연결 & Replication 설정
/////////////////////////////////

URogueActionSystemComponent* URogueActionBase::GetOwningComponent() const
{
	return Cast<URogueActionSystemComponent>(GetOuter());
}


ACharacter* URogueActionBase::GetOwningCharacter() const
{
	return Cast<ACharacter>(GetOuter()->GetOuter());
}


void URogueActionBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(URogueActionBase, bIsRunning);
}
