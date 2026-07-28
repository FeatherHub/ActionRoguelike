#include "RogueGameplayLayoutWidget.h"

#include "RogueActionWidget.h"
#include "Blueprint/WidgetTree.h"

void URogueGameplayLayoutWidget::RebindActionSystem(URogueActionSystemComponent* ASC)
{
	ActionWidgetContainer->WidgetTree->ForEachWidget([ASC](UWidget* Widget)
	{
		if(URogueActionWidget* ActionWidget = Cast<URogueActionWidget>(Widget))
		{
			ActionWidget->RebindActionSystem(ASC);
		}
	});
}
