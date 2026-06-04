#include "RogueActionSystemComponent.h"

#include "ActionRoguelike.h"
#include "RogueActionBase.h"
#include "GameplayTagContainer.h"
#include "RogueActionEffect.h"
#include "RogueAttributeSet.h"
#include "Development/RogueNetUtil.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

TAutoConsoleVariable<bool> CVarAttributeDebugMsg { TEXT("rogue.asc.attribute.ShowMsg"), false,
	TEXT("Show ActionSystemComponent's Attribute related on-screen messages. 0=off, 1=on"), ECVF_Cheat };

URogueActionSystemComponent::URogueActionSystemComponent()
{
	SetIsReplicatedByDefault(true);
	
	// debugging purpose
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	
	bWantsInitializeComponent = true;
}

void URogueActionSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	///////////////
	// Action
	for (TSubclassOf<URogueActionBase> ActionClass : DefaultGrantActions)
	{
		if (ActionClass)
		{
			GrantAction(ActionClass);
		}
	}
	
	///////////////
	// Attribute
	if (!AttributeSet)
	{
		AttributeSet = NewObject<URogueAttributeSet>(this);
	}
	
	for (TFieldIterator<FProperty> PropertyIt(AttributeSet->GetClass()); PropertyIt; ++PropertyIt)
	{
		FProperty* Property = *PropertyIt;
		
		FName AttributeTagName = FName("Attribute." + Property->GetName());
		FGameplayTag AttributeTag = FGameplayTag::RequestGameplayTag(AttributeTagName);
		
		FRogueAttribute* Attribute = Property->ContainerPtrToValuePtr<FRogueAttribute>(AttributeSet);
		
		CachedAttributeMap.Add(AttributeTag, Attribute);
	}
}

void URogueActionSystemComponent::SetDefaultAttributeSet(TSubclassOf<URogueAttributeSet> AttributeSetClass)
{
	FObjectInitializer& ObjectInitializer = FObjectInitializer::Get();
	
	AttributeSet = Cast<URogueAttributeSet>(ObjectInitializer.CreateDefaultSubobject(this, TEXT("AttributeSet"), AttributeSetClass, AttributeSetClass));
}

void URogueActionSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	
	AttributeSet->PostInitializeComponents();
}

void URogueActionSystemComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FString ActionMsg = FString::Printf(TEXT("Character(%s) has Actions: "), *GetNetDebugName(GetOwner()));
	for (URogueActionBase* Action : GrantedActions)
	{
		ActionMsg += FString::Printf(TEXT("%s | "), *Action->GetActionName().GetTagLeafName().ToString());
	}
	// DEBUG_NET_ONSCREEN(ActionMsg);
	ROGUE_DEBUG(0, ActionMsg, 0.f, FColor::White);
}

void URogueActionSystemComponent::GrantAction(TSubclassOf<URogueActionBase> ActionClass)
{
	URogueActionBase* NewAction = NewObject<URogueActionBase>(this, ActionClass);
	
	GrantedActions.Add(NewAction);
	
	if (NewAction->IsA(URogueActionEffect::StaticClass()))
	{
		ensureMsgf(NewAction->CanStart(), TEXT("Effect CanStart returns FALSE"));
		NewAction->StartAction();
	}
}

void URogueActionSystemComponent::RemoveAction(URogueActionBase* Action)
{
	int32 RemoveCount = GrantedActions.RemoveSingle(Action);
	ensure(RemoveCount == 1);
}

void URogueActionSystemComponent::ServerStartAction_Implementation(FGameplayTag ActionName)
{
	StartAction(ActionName);
}

void URogueActionSystemComponent::StartAction(FGameplayTag ActionName)
{
	for (URogueActionBase* Action : GrantedActions)
	{
		if (ActionName == Action->GetActionName())
		{
			if (Action->CanStart())
			{
				if(!GetOwner()->HasAuthority())
				{
					ServerStartAction(ActionName);					
				}
				Action->StartAction();
			}
			return;
		}
	}
	
	UE_LOGFMT(LogGame, Warning, "Failed to Start Action '{ActionName}'", ActionName.GetTagName());
}

void URogueActionSystemComponent::ServerStopAction_Implementation(FGameplayTag ActionName)
{
	StopAction(ActionName);
}

void URogueActionSystemComponent::StopAction(FGameplayTag ActionName)
{
	for (URogueActionBase* Action : GrantedActions)
	{
		if (ActionName == Action->GetActionName())
		{
			if (Action->CanStop())
			{
				if(!GetOwner()->HasAuthority())
				{
					ServerStopAction(ActionName);	
				}
				Action->StopAction();
			}
			return;
		}
	}
	
	UE_LOGFMT(LogGame, Warning, "Failed to Stop Action '{ActionName}'", ActionName.GetTagName());
}

bool URogueActionSystemComponent::ApplyAttributeChange(FGameplayTag AttributeTag, float InValue, EAttributeChangeType ChangeType)
{
	FRogueAttribute* Attribute = GetAttribute(AttributeTag);
	if (!Attribute)
	{
		return false;
	}
	float OldValue = Attribute->GetValue();
	
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
	case MultiplierDelta:
		Attribute->Multiplier += InValue;
		break;
	case MultiplierOverride:
		Attribute->Multiplier = InValue;
		break;
	default:
		check(false)
		break;
	}
	
	AttributeSet->PostApplyChange();

	float NewValue = Attribute->GetValue();
	
	bool bHasChanged = false;
	if (!FMath::IsNearlyEqual(NewValue, OldValue))
	{
		bHasChanged = true;
		MulticastAttributeChanged(AttributeTag, NewValue, OldValue);
	}
	
	FString AttrChangedMsg = FString::Printf(TEXT("[ASC::ApplyAttrChange] Character %s Attribute %s New %-6.0f Old %-6.0f"), 
		*GetNetDebugName(GetOwner()), *AttributeTag.GetTagLeafName().ToString(), NewValue, OldValue);
	// DEBUG_NET_ONSCREEN_EX_CVAR(CVarAttributeDebugMsg, AttrChangedMsg, 10.f, AttributeTag);
	ROGUE_DEBUG_CVAR(CVarAttributeDebugMsg,AttributeTag, AttrChangedMsg, 3.f, FColor::Orange);
	
	UE_LOG(LogGame, Log, TEXT("[%s]-[%s] New: %-6.1f, Old: %-6.1f Type: %s"), 
		*GetFNameSafe(GetOuter()).ToString().Left(25), *AttributeTag.ToString(), NewValue, OldValue, *UEnum::GetValueAsString(ChangeType));

	return bHasChanged;
}

void URogueActionSystemComponent::MulticastAttributeChanged_Implementation(FGameplayTag AttributeTag, float NewValue, float OldValue)
{
	FString AttrChangedMsg = FString::Printf(TEXT("[ASC::MulticastAttrChanged] %s Attr %s New %f Old %f"), 
		*GetNetDebugName(GetOwner()), *AttributeTag.ToString(), NewValue, OldValue);
	// DEBUG_NET_ONSCREEN_EX_CVAR(CVarAttributeDebugMsg, AttrChangedMsg, 10.f, AttributeTag);
	ROGUE_DEBUG_CVAR(CVarAttributeDebugMsg, AttributeTag, AttrChangedMsg, 5.f, FColor::Orange);
	
	// Native C++ Listeners
	if (FOnAttributeChanged* NativeListener = OnAttributeChangedListeners.Find(AttributeTag))
	{
		NativeListener->Broadcast(NewValue, OldValue);
	}

	// Dynamic Blueprint Listeners
	if (TArray<FOnAttributeChanged_Dynamic>* DynamicListeners = OnAttributeChangedListeners_Dynamic.Find(AttributeTag))
	{
		for (int i = DynamicListeners->Num() - 1; i >= 0; --i)
		{
			FOnAttributeChanged_Dynamic& Listener = (*DynamicListeners)[i];
				
			bool bIsBound = Listener.ExecuteIfBound(NewValue, OldValue);
			if (!bIsBound)
			{
				DynamicListeners->RemoveAt(i);
					
				UE_LOG(LogGame, Log, TEXT("Successfully removed unbound OnAttributeChanged_Dynamic for %s")
					, *AttributeTag.ToString());
			}
		}			
	}
}

FOnAttributeChanged& URogueActionSystemComponent::GetOnAttributeChangedListener(FGameplayTag AttributeTag)
{
	return OnAttributeChangedListeners.FindOrAdd(AttributeTag);
}

FRogueAttribute* URogueActionSystemComponent::GetAttribute(FGameplayTag AttributeTag) const
{
	FRogueAttribute* const* FoundAttribute = CachedAttributeMap.Find(AttributeTag);
	
	if (FoundAttribute)
	{
		return *FoundAttribute;
	}
	
	UE_LOG(LogGame, Warning, TEXT("Attribute %s not found on %s's ActionSystemComponent")
		, *AttributeTag.ToString(), *GetNameSafe(GetOuter()));
	
	return nullptr;
}

float URogueActionSystemComponent::GetAttributeValue(FGameplayTag AttributeTag) const
{
	FRogueAttribute* Attribute = GetAttribute(AttributeTag);
	if (Attribute)
	{
		return Attribute->GetValue();
	}
	
	ensure(false);
	return 0.f;
}

void URogueActionSystemComponent::AddOnAttributeChangedListener_Dynamic(FGameplayTag AttributeTag, FOnAttributeChanged_Dynamic OnAttributeChanged)
{
	TArray<FOnAttributeChanged_Dynamic>& Listeners = OnAttributeChangedListeners_Dynamic.FindOrAdd(AttributeTag);
	Listeners.Add(OnAttributeChanged);
}

void URogueActionSystemComponent::RemoveOnAttributeChangedListener_Dynamic(FOnAttributeChanged_Dynamic ListenerToRemove)
{
	for (TPair<FGameplayTag, TArray<FOnAttributeChanged_Dynamic>>& ListenersEntry : OnAttributeChangedListeners_Dynamic)
	{
		if (ListenersEntry.Value.RemoveSingle(ListenerToRemove) > 0)
		{
			UE_LOG(LogGame, Log, TEXT("Successfully removed OnAttributeChanged_Dynamic for %s")
				, *ListenersEntry.Key.ToString())

			return;
		}
	}
}

bool URogueActionSystemComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWrote = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	bWrote |= Channel->ReplicateSubobject(AttributeSet, *Bunch, *RepFlags);
	ensure(AttributeSet);

	for (URogueActionBase* Action : GrantedActions)
	{
		bWrote |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);
		ensure(Action);
	}
	
	return bWrote;
}

void URogueActionSystemComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(URogueActionSystemComponent, AttributeSet);
	DOREPLIFETIME(URogueActionSystemComponent, GrantedActions);
}
