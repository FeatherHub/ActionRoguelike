#include "RogueSkillWidget.h"

#include "ActionSystem/RogueActionBase.h"
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
	ActionIconMID->SetTextureParameterValue(TEXT("ActionIcon"), ActionCDO->GetActionIcon());
}

void URogueSkillWidget::BindActionSystem(URogueActionSystemComponent* ASC)
{
	//
}

void URogueSkillWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}
