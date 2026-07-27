#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ActionSystem/RogueActionType.h"
#include "Blueprint/UserWidget.h"
#include "RogueSkillWidget.generated.h"

class URogueActionSystemComponent;
class URogueActionBase;
class UImage;

UCLASS(Abstract)
class ACTIONROGUELIKE_API URogueSkillWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ActionIconImage;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> ActivationFailedAnim;
	
	UPROPERTY(EditAnywhere, Category=Action)
	TSubclassOf<URogueActionBase> ActionClass;

	UPROPERTY(VisibleDefaultsOnly, Category="Material Contract")
	FName ActionIconTextureParamName = TEXT("ActionIconTexture");

	UPROPERTY(VisibleDefaultsOnly, Category="Material Contract")
	FName CooldownProgressScalarParamName = TEXT("CooldownProgress");
	float LastCooldownProgress = -1.f;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Material Contract")
	FName SkillAvailableScalarParamName = TEXT("SkillAvailable");
	float LastSkillAvailable = -1.f;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Material Contract")
	FName RunningHighlightScalarParamName = TEXT("RunningHighlight");
	float LastSkillRunning = -1.f;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ActionIconMID;
	
	TWeakObjectPtr<URogueActionBase> BoundAction;
	TWeakObjectPtr<URogueActionSystemComponent> BoundASC;
	FGameplayTag ActionName;
	
public:
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void BindActionSystem(URogueActionSystemComponent* ASC);
	
protected:
	void ApplyStaticActionData();
	
	UFUNCTION()
	void OnGrantedActionChanged();
	
	void OnStartActionFailed(const FRogueCanStartResult& Result);
	
#if WITH_EDITOR
	void ValidateAssetSetup() const;
#endif
};
