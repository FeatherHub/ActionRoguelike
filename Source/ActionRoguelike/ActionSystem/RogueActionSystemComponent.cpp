#include "RogueActionSystemComponent.h"

#include "RogueActionBase.h"
#include "GameplayTagContainer.h"
#include "RogueAttributeSet.h"

URogueActionSystemComponent::URogueActionSystemComponent()
{
	bWantsInitializeComponent = true;
	
	AttributeSetClass = URogueAttributeSet::StaticClass();
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
	
	
	AttributeSet = NewObject<URogueAttributeSet>(this, AttributeSetClass);
	
	for (TFieldIterator<FProperty> PropertyIt(AttributeSetClass); PropertyIt; ++PropertyIt)
	{
		FProperty* Property = *PropertyIt;
		
		FName AttributeTagName = FName("Attribute." + Property->GetName());
		FGameplayTag AttributeTag = FGameplayTag::RequestGameplayTag(AttributeTagName);
		
		FRogueAttribute* Attribute = Property->ContainerPtrToValuePtr<FRogueAttribute>(AttributeSet);
		
		CachedAttributeMap.Add(AttributeTag, Attribute);
	}
}

void URogueActionSystemComponent::GrantAction(URogueActionBase* Action)
{
	GrantedActions.Add(Action);
}

void URogueActionSystemComponent::StartAction(FGameplayTag ActionName)
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
	
	UE_LOGFMT(LogTemp, Warning, "Failed to Start Action '{ActionName}'", ActionName.GetTagName());
}

void URogueActionSystemComponent::StopAction(FGameplayTag ActionName)
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
	
	UE_LOGFMT(LogTemp, Warning, "Failed to Stop Action '{ActionName}'", ActionName.GetTagName());
}

FOnAttributeChanged& URogueActionSystemComponent::GetOnAttributeChangedListener(FGameplayTag AttributeTag)
{
	return OnAttributeChangedListenerMap.FindOrAdd(AttributeTag);
}

FRogueAttribute* URogueActionSystemComponent::GetAttribute(FGameplayTag AttributeTag)
{
	return *CachedAttributeMap.Find(AttributeTag);
}

bool URogueActionSystemComponent::ApplyAttributeChange(FGameplayTag AttributeTag, float InValue, EAttributeChangeType ChangeType)
{
	FRogueAttribute* Attribute = GetAttribute(AttributeTag);

	float OldValue = Attribute->Base;
	
	switch (ChangeType) {
	case BaseDelta:
		Attribute->Base += InValue;
		break;
	case ModifierDelta:
		Attribute->Modifier += InValue;
		break;
	case BaseOverride:
		Attribute->Base = InValue;
		break;
	default:
		check(false)
		break;
	}
	
	AttributeSet->PostApplyChange();

	float NewValue = Attribute->Base;
	
	bool bHasChanged = false;
	if (!FMath::IsNearlyEqual(NewValue, OldValue))
	{
		bHasChanged = true;
		GetOnAttributeChangedListener(AttributeTag).Broadcast(NewValue, OldValue);
	}
	
	UE_LOG(LogTemp, Log, TEXT("[%s] New: %-6.1f, In: %-6.1f Type: %d")
		, *AttributeTag.ToString(), NewValue, InValue, ChangeType);

	return bHasChanged;
}
