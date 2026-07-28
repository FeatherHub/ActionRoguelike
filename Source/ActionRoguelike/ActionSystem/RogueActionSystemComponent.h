#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RogueActionType.h"
#include "Components/ActorComponent.h"
#include "RogueActionSystemComponent.generated.h"


struct FRogueAttribute;
class URogueAttributeSet;
class URogueActionBase;
class URogueActionEffect;
class URogueActionSystemComponent;
struct FGameplayTag;


UENUM()
enum ERogueAttributeModType
{
	BaseDelta,
	ModifierDelta,
	BaseOverride,
	MultiplierDelta,
	MultiplierOverride,
};


DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAttributeChanged, float /*NewValue*/, float /*OldValue*/);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAttributeChanged_Dynamic, float, NewValue, float, OldValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionEffectUpdated, URogueActionEffect*, UpdatedActionEffect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrantedActionChanged);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStartActionFailed, const FRogueCanStartResult&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnActionStopped, const FRogueStopActionInfo&);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatusTagUpdated, FGameplayTag, UpdatedTag, int32, NewCount);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROGUELIKE_API URogueActionSystemComponent : public UActorComponent
{
	GENERATED_BODY()

///////////
// Setup
///////////
public:
	URogueActionSystemComponent();
	virtual void InitializeComponent() override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


////////////////////
// 소유한 Action 관리
////////////////////
public:
	UPROPERTY(BlueprintAssignable)
	FOnActionEffectUpdated OnActionEffectAdded;
	
	UPROPERTY(BlueprintAssignable)
	FOnActionEffectUpdated OnActionEffectRemoved;
	
	UPROPERTY(BlueprintAssignable)
	FOnGrantedActionChanged OnGrantedActionChanged;

	UFUNCTION(BlueprintCallable)
	void GrantAction(TSubclassOf<URogueActionBase> ActionClass);
	
	UFUNCTION(BlueprintCallable)
	void RemoveAction(URogueActionBase* Action);
	
	UFUNCTION(BlueprintCallable)
	URogueActionBase* FindActionByTag(FGameplayTag ActionTag);
	

protected:
	UPROPERTY(EditDefaultsOnly, Category=Action)
	TArray<TSubclassOf<URogueActionBase>> DefaultGrantActions;

	UPROPERTY(ReplicatedUsing=OnRep_GrantedAction, EditAnywhere, Category=Action)
	TArray<TObjectPtr<URogueActionBase>> GrantedActions;

	UFUNCTION()
	void OnRep_GrantedAction();
	

///////////////////////
// Start & Stop Action
///////////////////////
public:
	FOnStartActionFailed OnStartActionFailed;
	FOnActionStopped OnActionStopped;

	void StartAction(FGameplayTag ActionTag);

	UFUNCTION(Server, Reliable)
	void StartAction_Server(FGameplayTag ActionTag);
	
	// 플레이어 입력 경로. 클라이언트에서 호출하면 서버를 호출하지 않는다.
	void StopAction(FGameplayTag ActionTag);

	UFUNCTION(Server, Reliable)
	void StopAction_Server(FGameplayTag ActionTag);
	
	// 게임 플레이 로직에 의한 중지. 각 머신에서 독립적으로 일어나므로 서버를 호출하지 않는다.
	UFUNCTION(BlueprintCallable)
	void InterruptAction(URogueActionBase* Action, const FRogueStopActionCause& Cause);
	
private:
	void StopActionInternal(URogueActionBase* Action, const FRogueStopActionCause& Cause);
	void InterruptActionsBlockedBy(const FGameplayTagContainer& NewTags);
	
	
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
	bool ApplyAttributeChange(FGameplayTag AttributeTag, float InValue, ERogueAttributeModType ChangeType);
	
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
// Status Tags
//////////////////
public:
	UPROPERTY(BlueprintAssignable)
	FOnStatusTagUpdated OnStatusTagUpdated;
	
	void AddStatusTags(const FGameplayTagContainer& TagsToAdd);
	void RemoveStatusTags(const FGameplayTagContainer& TagsToRemove);
	const FGameplayTagContainer& GetStatusTags() const
	{
		return StatusTags;
	}
	
protected:
	UPROPERTY(VisibleAnywhere, Category=Status)
	FGameplayTagContainer StatusTags;
};
