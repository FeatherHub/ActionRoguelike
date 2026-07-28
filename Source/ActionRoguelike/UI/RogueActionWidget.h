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

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ActionIconMID;
	
	TWeakObjectPtr<URogueActionBase> BoundAction;

	TWeakObjectPtr<URogueActionSystemComponent> BoundASC;
	
	FGameplayTag ActionTag;
	
	// 액션의 쿨다운 시간에 따른 위젯 효과
	UPROPERTY(VisibleDefaultsOnly, Category="Material Contract")
	FName CooldownProgress_ScalarParamName = TEXT("CooldownProgress");
	float CooldownProgress_LastVal = -1.f;
	
	// 위젯에 바인드된 액션을 ASC가 보유했는지 여부에 따른 위젯 효과
	UPROPERTY(VisibleDefaultsOnly, Category="Material Contract")
	FName ActionAvailable_ScalarParamName = TEXT("ActionAvailable");
	float ActionAvailable_LastVal = -1.f;
	
//////////////////
// 지속형 액션 효과
//////////////////
	UPROPERTY(VisibleDefaultsOnly, Category="Material Contract")
	FName RunningHighlight_ScalarParamName = TEXT("RunningHighlight");

	// 정상 종료 시 하이라이트가 빠지는 속도
	UPROPERTY(EditAnywhere, Category=UI, meta=(ClampMin="1.0"))
	float RunningHighlight_NormalFadeSpeed = 6.f;
	
	// Block 으로 종료 시 하이라이트가 빠지는 속도
	UPROPERTY(EditAnywhere, Category=UI, meta=(ClampMin="1.0"))
	float RunningHighlight_CancelFadeSpeed = 12.f;
	
	float RunningHighlight_SelectedFadeSpeed = RunningHighlight_NormalFadeSpeed; // 이번 페이드에 사용할 속도
	float RunningHighlight_CurrentInterpVal = 0.f; // 보간 중인 값
	float RunningHighlight_LastInterpVal = -1.f; // MID 에 마지막으로 적용한 값
	
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
