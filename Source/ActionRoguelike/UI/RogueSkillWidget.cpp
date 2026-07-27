#include "RogueSkillWidget.h"

#include "ActionRoguelike.h"
#include "ActionSystem/RogueActionBase.h"
#include "ActionSystem/RogueActionSystemComponent.h"
#include "Components/Image.h"
#include "Development/RogueLibrary.h"


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
	if(BoundASC == ASC)
	{
		return;
	}
	BoundASC = ASC;
	
	BoundASC->OnGrantedActionChanged.AddDynamic(this, &ThisClass::OnGrantedActionChanged);
	
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
