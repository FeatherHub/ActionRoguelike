#include "RogueActionBase.h"

#include "ActionRoguelike.h"
#include "RogueActionSystemComponent.h"
#include "RogueActionType.h"
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
		*ActionTag.ToString(), *NetUtil::GetNetName(this), *NetUtil::GetNetName(Character), *NetUtil::GetNetName(Character->GetController()));
	
	
	if(CooldownStartPolicy == ERogueCooldownPolicy::OnStart)
	{
		StartCooldown();
	}
	
	URogueActionSystemComponent* ASC = GetOwningComponent();
	ASC->AppendActiveTags(ActivationGrantTags);

	UE_LOGFMT(LogGame, Log, "Start Action '{ActionTag}' at {GameTime}", ActionTag.GetTagName(), GetWorld()->TimeSeconds);

	for (const TPair<FGameplayTag, float>& CostEntry : ActivationCostMap)
	{
		ASC->ApplyAttributeChange(CostEntry.Key, -CostEntry.Value, BaseDelta);
	}
	
	bIsRunning = true;
}


void URogueActionBase::StopAction_Implementation()
{
	bIsRunning = false;
	
	if(CooldownStartPolicy == ERogueCooldownPolicy::OnStop)
	{
		StartCooldown();
	}
	
	URogueActionSystemComponent* ASC = GetOwningComponent();
	ASC->RemoveActiveTags(ActivationGrantTags);
	
	UE_LOGFMT(LogGame, Log, 
		"Stop Action '{ActionTag}' at {GameTime}", ActionTag.GetTagName(), GetWorld()->TimeSeconds);
}


bool URogueActionBase::CanStart() const
{
	FRogueCanStartResult Dummy;
	return CanStart(Dummy);
}

bool URogueActionBase::CanStart(FRogueCanStartResult& OutResult) const
{
	OutResult = FRogueCanStartResult{};
	OutResult.ActionTag = ActionTag;
	OutResult.ActionClass = GetClass();
	
	if(IsRunning())
	{
		OutResult.Failure = ERogueCanStartFailure::Running;
		return false;
	}
	
	float CooldownRemaining = GetCooldownRemaining(); 
	if(CooldownRemaining > 0.f)
	{
		OutResult.Failure = ERogueCanStartFailure::Cooldown;
		OutResult.CooldownRemaining = CooldownRemaining;
		return false;
	}
	
	URogueActionSystemComponent* ASC = GetOwningComponent();
	if(ASC->GetActiveTags().HasAny(ActivationBlockingTags))
	{
		OutResult.Failure = ERogueCanStartFailure::Blocked;
		OutResult.BlockedByTags = ASC->GetActiveTags().Filter(ActivationBlockingTags);
		return false;
	}

	for (const TPair<FGameplayTag, float>& CostEntry : ActivationCostMap)
	{
		if(FRogueAttribute* Attribute = ASC->GetAttribute(CostEntry.Key))
		{
			float CurrentAmount = Attribute->GetValue();
			if(CurrentAmount < CostEntry.Value)
			{
				OutResult.Failure = ERogueCanStartFailure::Cost;
				OutResult.CostAttribute = CostEntry.Key;
				OutResult.CostRequired = CostEntry.Value;
				OutResult.CostCurrent = CurrentAmount;
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
		UE_LOGFMT(LogGame, Error, "Cannot Stop Action {ActionTag}. Becuase it is not in running.", ActionTag.GetTagName());
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
