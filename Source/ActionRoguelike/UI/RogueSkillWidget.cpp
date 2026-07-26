#include "RogueSkillWidget.h"

#include "ActionSystem/RogueActionBase.h"
#include "ActionSystem/RogueActionSystemComponent.h"
#include "Components/Image.h"


void URogueSkillWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if(!ActionClass || !ActionIconImage)
	{
		return;
	}
	
	const URogueActionBase* ActionCDO = GetDefault<URogueActionBase>(ActionClass);
	ActionName = ActionCDO->GetActionName();
	
	ActionIconMID = ActionIconImage->GetDynamicMaterial();
	if(ActionIconMID)
	{
		ActionIconMID->SetTextureParameterValue(TEXT("ActionIconTexture"), ActionCDO->GetActionIcon());
	}
}

void URogueSkillWidget::BindActionSystem(URogueActionSystemComponent* ASC)
{
	if(BoundASC == ASC)
	{
		return;
	}
	BoundASC = ASC;
	
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
	
	if(FMath::IsNearlyEqual(CooldownProgress, LastCooldownProgress))
	{
		return;
	}
	
	ActionIconMID->SetScalarParameterValue(TEXT("CooldownProgress"), CooldownProgress);
	LastCooldownProgress = CooldownProgress;
}
