#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "RogueActionSystemComponent.generated.h"


struct FRogueAttribute;
class URogueAttributeSet;
class URogueActionBase;
class URogueActionSystemComponent;
struct FGameplayTag;

UENUM()
enum EAttributeChangeType
{
	BaseDelta,
	ModifierDelta,
	BaseOverride,
	MultiplierDelta,
	MultiplierOverride,
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAttributeChanged, float /*NewValue*/, float /*OldValue*/);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAttributeChanged_Dynamic, float, NewValue, float, OldValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameplayTagUpdated, FGameplayTag, UpdatedTag, int32, NewCount);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROGUELIKE_API URogueActionSystemComponent : public UActorComponent
{
	GENERATED_BODY()
	
	////////////////
	// Life Cycle
	////////////////
public:
	URogueActionSystemComponent();
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	
	///////////////
	// Action
	///////////////
public:
	UFUNCTION(Server, Reliable)
	void ServerStartAction(FGameplayTag ActionName);
	void StartAction(FGameplayTag ActionName);
	
	UFUNCTION(Server, Reliable)
	void ServerStopAction(FGameplayTag ActionName);
	void StopAction(FGameplayTag ActionName);
	
	UFUNCTION(BlueprintCallable)
	void GrantAction(TSubclassOf<URogueActionBase> ActionClass);

	void RemoveAction(URogueActionBase* Action);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category=Action)
	TArray<TSubclassOf<URogueActionBase>> DefaultGrantActions;

	UPROPERTY(Replicated, EditAnywhere, Category=Action)
	TArray<TObjectPtr<URogueActionBase>> GrantedActions;

	
	//////////////
	// Attribute
	//////////////
public:
	void SetDefaultAttributeSet(TSubclassOf<URogueAttributeSet> AttributeSetClass);
	FRogueAttribute* GetAttribute(FGameplayTag AttributeTag) const;
	FOnAttributeChanged& GetOnAttributeChangedListener(FGameplayTag AttributeTag);

	UFUNCTION(BlueprintCallable)
	float GetAttributeValue(FGameplayTag AttributeTag) const;
	
	UFUNCTION(BlueprintCallable)
	bool ApplyAttributeChange(FGameplayTag AttributeTag, float InValue, EAttributeChangeType ChangeType);
	
	UFUNCTION(BlueprintCallable, DisplayName="Add Attribute Changed Listener", meta=(Keywords="Event, Delegate"))
	void AddOnAttributeChangedListener_Dynamic(FGameplayTag AttributeTag, FOnAttributeChanged_Dynamic OnAttributeChanged);
	
	UFUNCTION(BlueprintCallable, DisplayName="Remove Attribute Changed Listener", meta=(Keywords="Event, Delegate"))
	void RemoveOnAttributeChangedListener_Dynamic(FOnAttributeChanged_Dynamic ListenerToRemove);
	
protected:
	UPROPERTY(Replicated, EditAnywhere, Instanced, Category=Attribute)
	TObjectPtr<URogueAttributeSet> AttributeSet;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastAttributeChanged(FGameplayTag AttributeTag, float NewValue, float OldValue);
	
	TMap<FGameplayTag, FRogueAttribute*> CachedAttributeMap;
	
	TMap<FGameplayTag, FOnAttributeChanged> OnAttributeChangedListeners;

	TMap<FGameplayTag, TArray<FOnAttributeChanged_Dynamic>> OnAttributeChangedListeners_Dynamic;
	
	//////////////////
	// Gameplay Tags
	//////////////////
public:
	FOnGameplayTagUpdated OnGameplayTagUpdated;
	
	void AppendActiveTags(const FGameplayTagContainer& NewTags);
	void RemoveActiveTags(const FGameplayTagContainer& TagsToRemove);
	const FGameplayTagContainer& GetActiveTags() const
	{
		return ActiveTags;
	}
	
protected:
	UPROPERTY(VisibleAnywhere, Category=Action)
	FGameplayTagContainer ActiveTags;
	
	
public:
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
};
