#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "RogueActionBase.generated.h"

class URogueActionSystemComponent;
class ACharacter;

UCLASS(Abstract, Blueprintable)
class ACTIONROGUELIKE_API URogueActionBase : public UObject
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category=Action)
	FGameplayTagContainer ActivationBlockedTags;

	UPROPERTY(EditDefaultsOnly, Category=Action)
	FGameplayTagContainer ActivationGrantTags;
	
	UPROPERTY(EditDefaultsOnly, Category=Action)
	FGameplayTag ActionName;
	
	UPROPERTY(EditDefaultsOnly, Category=Action)
	float CooldownTime = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category=Action)
	TMap<FGameplayTag, float> ActivationCostMap;
	
	UPROPERTY(Transient)
	float CooldownEndTime = 0.f;
	
	UPROPERTY(ReplicatedUsing=OnRep_IsRunning, Transient)
	bool bIsRunning = false;
	
	UFUNCTION()
	void OnRep_IsRunning();
	
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TObjectPtr<UTexture2D> ActionIcon;
	
	UPROPERTY(EditDefaultsOnly, Category=UI)
	FText DisplayName;
	
	UPROPERTY(EditDefaultsOnly, Category=UI)
	FText Description;
	
protected:
	UFUNCTION(BlueprintCallable)
	URogueActionSystemComponent* GetOwningComponent() const;

	UFUNCTION(BlueprintCallable)
	ACharacter* GetOwningCharacter() const;
	
#if WITH_EDITOR
	virtual bool ImplementsGetWorld() const override { return true; }
#endif 
	virtual bool IsSupportedForNetworking() const override { return true; }
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=Action)
	void StartAction();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable,Category=Action)
	void StopAction();
	
	bool CanStart() const;
	bool CanStop() const;
	bool IsRunning() const { return bIsRunning; }
	
	float GetCooldownRemaining() const;
	/**
	 *	- [0, 1] 범위의 값을 반환   
	 *	- 0 = 사용 가능. 1 = 액션 시작 직후     
	 */
	float GetCooldownProgress() const;
	const FGameplayTagContainer& GetBlockedTags() const { return ActivationBlockedTags; }
	FGameplayTag GetActionName() const { return ActionName; }
	UTexture2D* GetActionIcon() const { return ActionIcon; }
	FText GetDisplayName() const { return DisplayName; }
	FText GetDescription() const { return Description; }
};
