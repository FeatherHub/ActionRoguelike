#include "RogueActionWidget.h"

#include "ActionRoguelike.h"
#include "ActionSystem/RogueActionBase.h"
#include "ActionSystem/RogueActionSystemComponent.h"
#include "Components/Image.h"
#include "DebugSystem/DebugUtil.h"
#include "Development/RogueLibrary.h"


static TAutoConsoleVariable<bool> CVarActionWidgetShowMessage { TEXT("rogue.ui.actionwidget.ShowMessage"), false,
	TEXT("Display Action Widget debug messages on screen. 0=off, 1=on"), ECVF_Default
};


void URogueActionWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	ApplyStaticActionData();
	
	LastCooldownProgress = -1.f;
	LastActionAvailable = -1.f;
	LastRunningHighlight = -1.f;
	
#if WITH_EDITOR
	ValidateAssetSetup();
#endif
}


void URogueActionWidget::ApplyStaticActionData()
{
	if(!ActionClass || !ActionIconImage)
	{
		return;
	}
	
	const URogueActionBase* ActionCDO = GetDefault<URogueActionBase>(ActionClass);
	ActionTag = ActionCDO->GetActionTag();
	
	ActionIconMID = ActionIconImage->GetDynamicMaterial();
	if(ActionIconMID)
	{
		ActionIconMID->SetTextureParameterValue(ActionIconTextureParamName, ActionCDO->GetActionIcon());
	}
}


void URogueActionWidget::RebindActionSystem(URogueActionSystemComponent* ASC)
{
	// RebindActionSystem이 중복으로 호출된 경우 방어
	if(BoundASC == ASC)
	{
		return;
	}
	
	UnbindActionSystem();

	if(!IsValid(ASC))
	{
		DEBUG_ONSCREEN_CVAR(CVarActionWidgetShowMessage, 0, 5.f, FColor::Red, TEXT("[URogueActionWidget] Invalid ASC is passed to RebindActionSystem"));
		return;
	}

	BoundASC = ASC;

	BoundASC->OnGrantedActionChanged.AddDynamic(this, &ThisClass::OnGrantedActionChanged);
	BoundASC->OnStartActionFailed.AddUObject(this, &ThisClass::OnStartActionFailed);
	BoundASC->OnActionStopped.AddUObject(this, &ThisClass::OnActionStopped);
	
	URogueActionBase* FoundAction = BoundASC->FindActionByTag(ActionTag);
	BoundAction = FoundAction;
}


void URogueActionWidget::UnbindActionSystem()
{
	if(URogueActionSystemComponent* ASC = BoundASC.Get())
	{
		ASC->OnGrantedActionChanged.RemoveAll(this);
		ASC->OnStartActionFailed.RemoveAll(this);
		ASC->OnActionStopped.RemoveAll(this);
	}
	BoundASC.Reset();
	BoundAction.Reset();
}


void URogueActionWidget::NativeDestruct()
{
	UnbindActionSystem();
	Super::NativeDestruct();
}


void URogueActionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if(!ActionIconMID)
	{
		return;
	}
	
	URogueActionBase* Action = BoundAction.Get();
	const float CooldownProgress = Action ? Action->GetCooldownProgress() : 0.f;
	const float ActionAvailable = Action ? 1.f : 0.f;
	const float RunningHighlight = (Action && Action->IsDurationAction() && Action->IsRunning()) ? 1.f : 0.f;
	
	RogueLibrary::ApplyScalarParameter(ActionIconMID, CooldownProgressScalarParamName, CooldownProgress, LastCooldownProgress);
	RogueLibrary::ApplyScalarParameter(ActionIconMID, ActionAvailableScalarParamName, ActionAvailable, LastActionAvailable);
	RogueLibrary::ApplyScalarParameter(ActionIconMID, RunningHighlightScalarParamName, RunningHighlight, LastRunningHighlight);
}


void URogueActionWidget::OnGrantedActionChanged()
{
	URogueActionBase* FoundAction = BoundASC->FindActionByTag(ActionTag);
	BoundAction = FoundAction;
}


void URogueActionWidget::OnStartActionFailed(const FRogueCanStartResult& Result)
{
	if(Result.ActionTag != ActionTag)
	{
		return;
	}
	
	// 미보유 스킬의 경우
	if(!BoundAction.IsValid())
	{
		return;
	}
	
	PlayShakeAnim();
}


void URogueActionWidget::OnActionStopped(const FRogueStopActionInfo& Info)
{
	if(Info.StoppedActionTag != ActionTag)
	{
		return;
	}
	
	if(Info.Cause.Reason == ERogueStopActionReason::BlockedByTag)
	{
		PlayShakeAnim();
	}
}


void URogueActionWidget::PlayShakeAnim()
{
	if(!ShakeAnim)
	{
		return;
	}
	
	PlayAnimation(ShakeAnim);
}


#if WITH_EDITOR

void URogueActionWidget::ValidateAssetSetup() const
{
	if (!ActionClass)
	{
		UE_LOGFMT(LogGame, Error, "[{Widget}] ActionClass 가 지정되지 않았습니다.", GetName());
		return;
	}

	if (!ActionIconImage)
	{
		UE_LOGFMT(LogGame, Error, "[{Widget}] ActionIconImage 위젯을 찾을 수 없습니다.", GetName());
		return;
	}
	
	const UMaterialInterface* BrushMaterial = Cast<UMaterialInterface>(ActionIconImage->GetBrush().GetResourceObject());
	if (!BrushMaterial)
	{
		UE_LOGFMT(LogGame, Error,
			"[{Widget}] ActionIconImage 의 Brush 가 Material 이 아닙니다.",
			GetName());
		return;
	}

	const UMaterialInterface* BaseMaterial = BrushMaterial->GetMaterial();
	UTexture* DummyTexture = nullptr;
	if (!BaseMaterial->GetTextureParameterValue(FMaterialParameterInfo{ActionIconTextureParamName}, DummyTexture))
	{
		UE_LOGFMT(LogGame, Error, "[{Widget}] Material '{Material}' 에 Texture Parameter '{Param}' 이 없습니다.",
			GetName(), BaseMaterial->GetName(), ActionIconTextureParamName);
	}

	float DummyScalar = 0.f;
	TArray<FName> ScalarParamNames = {CooldownProgressScalarParamName, RunningHighlightScalarParamName, ActionAvailableScalarParamName};
	for (const FName& ScalarParamName : ScalarParamNames)
	{
		if (!BaseMaterial->GetScalarParameterValue(FMaterialParameterInfo{ScalarParamName}, DummyScalar))
		{
			UE_LOGFMT(LogGame, Error, "[{Widget}] Material '{Material}' 에 Scalar Parameter '{Param}' 이 없습니다.",
				GetName(), BaseMaterial->GetName(), ScalarParamName);
		}
	}
}

#endif
