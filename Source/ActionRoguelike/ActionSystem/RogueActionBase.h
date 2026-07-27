#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "RogueActionBase.generated.h"

class URogueActionSystemComponent;
class ACharacter;

UENUM()
enum class ECooldownStartPolicy : uint8
{
	/** StartAction 시점부터 쿨다운 계산 시작. e.g. 공격 발사 등 순간형 액션 */
	OnStart,
	/** StopAction 시점부터 쿨다운 계산 시작. e.g. 스프린트 등 지속형 액션 */
	OnStop
};

UCLASS(Abstract, Blueprintable)
class ACTIONROGUELIKE_API URogueActionBase : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category=Action)
	FGameplayTagContainer ActivationBlockedTags;

	UPROPERTY(EditDefaultsOnly, Category=Action)
	FGameplayTagContainer ActivationGrantTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Action)
	FGameplayTag ActionName;
	
	UPROPERTY(EditDefaultsOnly, Category=Action)
	TMap<FGameplayTag, float> ActivationCostMap;
	
	UPROPERTY(ReplicatedUsing=OnRep_IsRunning, Transient)
	bool bIsRunning = false;
	
	UFUNCTION()
	void OnRep_IsRunning();
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=Action)
	void StartAction();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable,Category=Action)
	void StopAction();
	
	bool CanStart() const;
	bool CanStop() const;
	bool IsRunning() const { return bIsRunning; }
	
	const FGameplayTagContainer& GetBlockedTags() const { return ActivationBlockedTags; }
	FGameplayTag GetActionName() const { return ActionName; }

	
/////////////
// Cooldown
protected:
	UPROPERTY(EditDefaultsOnly, Category=Action)
	float CooldownTime = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category=Action)
	ECooldownStartPolicy CooldownStartPolicy = ECooldownStartPolicy::OnStart;
	
	UPROPERTY(Transient)
	float CooldownEndTime = 0.f;
	
	void StartCooldown();
public:
	float GetCooldownRemaining() const;
	/**
	 *	- [0, 1] 범위의 값 반환   
	 *	- 0 = 사용 가능. 1 = 액션 시작 직후     
	 */
	float GetCooldownProgress() const;
	bool IsDurationAction() const { return CooldownStartPolicy == ECooldownStartPolicy::OnStop; }
	
	
/////////
// UI
protected:
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TObjectPtr<UTexture2D> ActionIcon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=UI)
	FText DisplayName;
	
	UPROPERTY(EditDefaultsOnly, Category=UI)
	FText Description;
	
public:
	UTexture2D* GetActionIcon() const { return ActionIcon; }
	FText GetDisplayName() const { return DisplayName; }
	FText GetDescription() const { return Description; }
	
	
/////////////////////////////////
// 변수 연결 & Replication 설정
protected:
	UFUNCTION(BlueprintCallable)
	URogueActionSystemComponent* GetOwningComponent() const;

	UFUNCTION(BlueprintCallable)
	ACharacter* GetOwningCharacter() const;
	
#if WITH_EDITOR
	virtual bool ImplementsGetWorld() const override { return true; }
#endif 
	virtual bool IsSupportedForNetworking() const override { return true; }
	
};
