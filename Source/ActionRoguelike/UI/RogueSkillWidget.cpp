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
	ActionName = ActionCDO->GetActionName();
	
	ActionIconMID = ActionIconImage->GetDynamicMaterial();
	if(ActionIconMID)
	{
		ActionIconMID->SetTextureParameterValue(ActionIconTextureParamName, ActionCDO->GetActionIcon());
	}
}


void URogueSkillWidget::BindActionSystem(URogueActionSystemComponent* ASC)
{
	// BindActionSystem이 중복으로 호출된 경우 방어
	if(BoundASC == ASC)
	{
		return;
	}
	
	// 컨트롤 중인 Pawn이 변경된 경우, 이전에 구독한 Delegate 해제
	if(URogueActionSystemComponent* OldASC = BoundASC.Get())
	{
		OldASC->OnGrantedActionChanged.RemoveAll(this);
		OldASC->OnStartActionFailed.RemoveAll(this);
	}
	
	BoundASC = ASC;
	BoundAction = nullptr;

	if(!IsValid(ASC))
	{
		DEBUG_ONSCREEN_CVAR(CVarSkillWidgetShowMessage, 0, 5.f, FColor::Red, TEXT("[URogueSkillWidget] Invalid ASC is passed to BindActionSystem"));
		return;
	}

	BoundASC->OnGrantedActionChanged.AddDynamic(this, &ThisClass::OnGrantedActionChanged);
	BoundASC->OnStartActionFailed.AddUObject(this, &ThisClass::OnStartActionFailed);
	
	URogueActionBase* FoundAction = BoundASC->FindActionByName(ActionName);
	BoundAction = FoundAction;
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
	URogueActionBase* FoundAction = BoundASC->FindActionByName(ActionName);
	BoundAction = FoundAction;
}


void URogueSkillWidget::OnStartActionFailed(const FRogueCanStartResult& Result)
{
	if(Result.ActionName != ActionName)
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
