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
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAttributeChanged, float /*NewValue*/, float /*OldValue*/);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROGUELIKE_API URogueActionSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category=Action)
	FGameplayTagContainer ActiveTags;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category=Action)
	TArray<TSubclassOf<URogueActionBase>> DefaultGrantActions;

	UPROPERTY(EditAnywhere, Category=Action)
	TArray<TObjectPtr<URogueActionBase>> GrantedActions;

	UPROPERTY()
	TObjectPtr<URogueAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, Category=Attribute)
	TSubclassOf<URogueAttributeSet> AttributeSetClass;
	
	TMap<FGameplayTag, FRogueAttribute*> CachedAttributeMap;
	
	TMap<FGameplayTag, FOnAttributeChanged> OnAttributeChangedListenerMap;

public:
	URogueActionSystemComponent();
	virtual void InitializeComponent() override;
	
	void StartAction(FGameplayTag ActionName);
	void StopAction(FGameplayTag ActionName);
	void GrantAction(URogueActionBase* Action);
	
	FOnAttributeChanged& GetOnAttributeChangedListener(FGameplayTag AttributeTag);
	FRogueAttribute* GetAttribute(FGameplayTag AttributeTag);
	bool ApplyAttributeChange(FGameplayTag AttributeTag, float InValue, EAttributeChangeType ChangeType);
};
