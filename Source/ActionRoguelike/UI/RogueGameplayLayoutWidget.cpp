#include "RogueGameplayLayoutWidget.h"

#include "RogueSkillWidget.h"
#include "Blueprint/WidgetTree.h"

void URogueGameplayLayoutWidget::RebindActionSystem(URogueActionSystemComponent* ASC)
{
	SkillWidgetContainer->WidgetTree->ForEachWidget([ASC](UWidget* Widget)
	{
		if(URogueSkillWidget* SkillWidget = Cast<URogueSkillWidget>(Widget))
		{
			SkillWidget->RebindActionSystem(ASC);
		}
	});
}
