#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ActionSystem/RogueActionType.h"
#include "Blueprint/UserWidget.h"
#include "RogueActionWidget.generated.h"

class URogueActionSystemComponent;
class URogueActionBase;
class UImage;

UCLASS(Abstract)
class ACTIONROGUELIKE_API URogueActionWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ActionIconImage;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> ShakeAnim;
	
	UPROPERTY(EditAnywhere, Category=Action)
	TSubclassOf<URogueActionBase> ActionClass;

	// ActionIconImage 에 적용할 Action Texture 
	UPROPERTY(VisibleDefaultsOnly, Category="Material Contract")
	FName ActionIconTextureParamName = TEXT("ActionIconTexture");

	// 액션의 쿨다운 시간에 따른 위젯 효과
	UPROPERTY(VisibleDefaultsOnly, Category="Material Contract")
	FName CooldownProgressScalarParamName = TEXT("CooldownProgress");
	float LastCooldownProgress;
	
	// 위젯에 바인드된 액션을 ASC가 보유했는지 여부에 따른 위젯 효과
	UPROPERTY(VisibleDefaultsOnly, Category="Material Contract")
	FName ActionAvailableScalarParamName = TEXT("ActionAvailable");
	float LastActionAvailable;
	
////////////////////
// 지속형 액션의 효과
////////////////////
	UPROPERTY(VisibleDefaultsOnly, Category="Material Contract")
	FName RunningHighlightScalarParamName = TEXT("RunningHighlight");
	
	float LastRunningHighlight; // MID 에 마지막으로 적용한 값
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ActionIconMID;
	
	TWeakObjectPtr<URogueActionBase> BoundAction;
	TWeakObjectPtr<URogueActionSystemComponent> BoundASC;
	FGameplayTag ActionTag;
	
	
public:
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void RebindActionSystem(URogueActionSystemComponent* ASC);
	void UnbindActionSystem();
	
	
protected:
	virtual void NativeDestruct() override;
	
	void ApplyStaticActionData();
	
	UFUNCTION()
	void OnGrantedActionChanged();
	
	void OnStartActionFailed(const FRogueCanStartResult& Result);
	
	void OnActionStopped(const FRogueStopActionInfo& Info);

	void PlayShakeAnim();
	
#if WITH_EDITOR
	void ValidateAssetSetup() const;
#endif
};
