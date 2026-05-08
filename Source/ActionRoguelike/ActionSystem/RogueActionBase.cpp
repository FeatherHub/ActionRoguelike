#include "RogueActionBase.h"

#include "RogueActionSystemComponent.h"
#include "RogueAttributeSet.h"
#include "Core/RogueGameplayTag.h"
#include "GameFramework/Character.h"

void URogueActionBase::StartAction_Implementation()
{
	bIsRunning = true;
	
	float CurrentTime = GetWorld()->TimeSeconds;
	CooldownEndTime = CurrentTime + CooldownTime;

	URogueActionSystemComponent* ASC = GetOwningComponent();
	ASC->ActiveTags.AppendTags(ActivationGrantTags);

	UE_LOGFMT(LogTemp, Log, "Start Action '{ActionName}' at {GameTime}", ActionName.GetTagName(), CurrentTime);
	
	if (FRogueAttribute* RageAttribute = GetOwningComponent()->GetAttribute(RogueGameplayTag::Attribute_RageAmount))
	{
		ASC->ApplyAttributeChange(RogueGameplayTag::Attribute_RageAmount, -RageCost, BaseDelta);
		
		UE_LOGFMT(LogTemp, Log, "Used {0} Rage. Remaining Rage: {1}", RageCost, RageAttribute->GetValue());
	}
}

void URogueActionBase::StopAction_Implementation()
{
	bIsRunning = false;
	
	URogueActionSystemComponent* ASC = GetOwningComponent();
	ASC->ActiveTags.RemoveTags(ActivationGrantTags);
	
	float GameTime = GetWorld()->TimeSeconds;
	UE_LOGFMT(LogTemp, Log, "Stop Action '{ActionName}' at {GameTime}", ActionName.GetTagName(), GameTime);
}

bool URogueActionBase::CanStart() const
{
	if (IsRunning())
	{
		UE_LOGFMT(LogTemp, Warning, "Cannot Start Action {ActionName}. Because it is in running", ActionName.GetTagName());
		return false;
	}
	
	if (FRogueAttribute* RageAttribute = GetOwningComponent()->GetAttribute(RogueGameplayTag::Attribute_RageAmount))
	{
		float CurrentRage = RageAttribute->GetValue(); 
		if (CurrentRage < RageCost)
		{
			UE_LOGFMT(LogTemp, Warning, "Cannot Start Action {0}. Because Rage is not enough. Needs: {1} Has: {2}"
			, ActionName.GetTagName(), RageCost, CurrentRage);
			return false;
		}
	}
	
	float CooldownRemaining = GetCooldownRemaining();
	if (CooldownRemaining > 0.f)
	{
		UE_LOGFMT(LogTemp, Warning, "Cannot Start Action {ActionName}. Because Cooldown remains: {CooldownRemaining}"
			, ActionName.GetTagName(), CooldownRemaining);
		return false;
	}

	URogueActionSystemComponent* ASC = GetOwningComponent();
	if (ASC->ActiveTags.HasAny(ActivationBlockedTags))
	{
		UE_LOGFMT(LogTemp, Warning, "Cannot Start Action {ActionName}. Because it blocks {BlockedTags}"
			, ActionName.GetTagName(), ActivationBlockedTags.ToString());
		return false;
	}

	return true;
}

bool URogueActionBase::CanStop() const
{
	if (!IsRunning())
	{
		UE_LOGFMT(LogTemp, Error, "Cannot Stop Action {ActionName}. Becuase it is not in running.", ActionName.GetTagName());
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
