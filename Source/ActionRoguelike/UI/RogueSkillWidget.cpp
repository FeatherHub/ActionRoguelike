#include "RogueSkillWidget.h"

#include "ActionRoguelike.h"
#include "ActionSystem/RogueActionBase.h"
#include "ActionSystem/RogueActionSystemComponent.h"
#include "Components/Image.h"
#include "DebugSystem/DebugUtil.h"
#include "Development/RogueLibrary.h"

static TAutoConsoleVariable<bool> CVarSkillWidgetShowMessage { TEXT("rogue.ui.skillwidget.ShowMessage"), false,
	TEXT("Display Skill Widget debug messages on screen. 0=off, 1=on"), ECVF_Default
};


void URogueSkillWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	ApplyStaticActionData();
	
	LastCooldownProgress = -1.f;
	LastSkillAvailable = -1.f;
	LastSkillRunning = -1.f;
	
#if WITH_EDITOR
	ValidateAssetSetup();
#endif
}


void URogueSkillWidget::ApplyStaticActionData()
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


void URogueSkillWidget::RebindActionSystem(URogueActionSystemComponent* ASC)
{
	// RebindActionSystem이 중복으로 호출된 경우 방어
	if(BoundASC == ASC)
	{
		return;
	}
	
	UnbindActionSystem();

	if(!IsValid(ASC))
	{
		DEBUG_ONSCREEN_CVAR(CVarSkillWidgetShowMessage, 0, 5.f, FColor::Red, TEXT("[URogueSkillWidget] Invalid ASC is passed to RebindActionSystem"));
		return;
	}

	BoundASC = ASC;

	BoundASC->OnGrantedActionChanged.AddDynamic(this, &ThisClass::OnGrantedActionChanged);
	BoundASC->OnStartActionFailed.AddUObject(this, &ThisClass::OnStartActionFailed);
	
	URogueActionBase* FoundAction = BoundASC->FindActionByTag(ActionTag);
	BoundAction = FoundAction;
}


void URogueSkillWidget::UnbindActionSystem()
{
	if(URogueActionSystemComponent* ASC = BoundASC.Get())
	{
		ASC->OnGrantedActionChanged.RemoveAll(this);
		ASC->OnStartActionFailed.RemoveAll(this);
	}
	BoundASC.Reset();
	BoundAction.Reset();
}


void URogueSkillWidget::NativeDestruct()
{
	UnbindActionSystem();
	Super::NativeDestruct();
}


void URogueSkillWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if(!ActionIconMID)
	{
		return;
	}
	
	URogueActionBase* Action = BoundAction.Get();
	const float CooldownProgress = Action ? Action->GetCooldownProgress() : 0.f;
	const float SkillAvailable = Action ? 1.f : 0.f;
	const float SkillRunning = (Action && Action->IsDurationAction() && Action->IsRunning()) ? 1.f : 0.f;
	
	RogueLibrary::ApplyScalarParamter(ActionIconMID, CooldownProgressScalarParamName, CooldownProgress, LastCooldownProgress);
	RogueLibrary::ApplyScalarParamter(ActionIconMID, SkillAvailableScalarParamName, SkillAvailable, LastSkillAvailable);
	RogueLibrary::ApplyScalarParamter(ActionIconMID, RunningHighlightScalarParamName, SkillRunning, LastSkillRunning);
}


void URogueSkillWidget::OnGrantedActionChanged()
{
	URogueActionBase* FoundAction = BoundASC->FindActionByTag(ActionTag);
	BoundAction = FoundAction;
}


void URogueSkillWidget::OnStartActionFailed(const FRogueCanStartResult& Result)
{
	if(Result.ActionTag != ActionTag)
	{
		return;
	}
	
	// 미보유 스킬의 경우 스킵
	if(!BoundAction.IsValid())
	{
		return;
	}
	
	if(!ActivationFailedAnim)
	{
		return;
	}
	
	PlayAnimation(ActivationFailedAnim);
}


#if WITH_EDITOR

void URogueSkillWidget::ValidateAssetSetup() const
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
	TArray<FName> ScalarParamNames = {CooldownProgressScalarParamName, RunningHighlightScalarParamName, SkillAvailableScalarParamName};
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
