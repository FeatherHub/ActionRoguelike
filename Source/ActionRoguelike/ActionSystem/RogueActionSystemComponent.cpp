#include "RogueActionSystemComponent.h"

#include "RogueActionBase.h"


URogueActionSystemComponent::URogueActionSystemComponent()
{
	bWantsInitializeComponent = true;
}

void URogueActionSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	for (TSubclassOf<URogueActionBase> ActionClass : DefaultGrantActions)
	{
		if (ActionClass)
		{
			URogueActionBase* NewAction = NewObject<URogueActionBase>(this, ActionClass);
			GrantAction(NewAction);
		}
	}
}

void URogueActionSystemComponent::GrantAction(URogueActionBase* Action)
{
	GrantedActions.Add(Action);
}

void URogueActionSystemComponent::StartAction(FName ActionName)
{
	for (URogueActionBase* Action : GrantedActions)
	{
		if (ActionName == Action->GetActionName())
		{
			if (Action->CanStart())
			{
				Action->StartAction();
			}
			return;
		}
	}
	
	UE_LOGFMT(LogTemp, Warning, "Failed to Start Action '{ActionName}'", ActionName);
}

void URogueActionSystemComponent::StopAction(FName ActionName)
{
	for (URogueActionBase* Action : GrantedActions)
	{
		if (ActionName == Action->GetActionName())
		{
			if (Action->CanStop())
			{
				Action->StopAction();
			}
			return;
		}
	}
	
	UE_LOGFMT(LogTemp, Warning, "Failed to Stop Action '{ActionName}'", ActionName);
}

bool URogueActionSystemComponent::ApplyHealthChange(float InHealthDelta)
{
	float OldHealth = AttributeSet.Health; 
	float NewHealth = FMath::Clamp(AttributeSet.Health + InHealthDelta, 0.f, AttributeSet.MaxHealth);
	
	bool bHasChanged = false;
	if (!FMath::IsNearlyEqual(OldHealth, NewHealth))
	{
		AttributeSet.Health = NewHealth;
		bHasChanged = true;
		OnHealthChanged.Broadcast(NewHealth, OldHealth);
	}
	
	UE_LOG(LogTemp, Log, TEXT("[HEALTH] Max: %-6.1f, New: %-6.1f, Delta: %-6.1f"), AttributeSet.MaxHealth, NewHealth, InHealthDelta);
	
	return bHasChanged;
}
