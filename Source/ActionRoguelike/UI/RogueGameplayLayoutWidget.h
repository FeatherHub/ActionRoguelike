#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RogueGameplayLayoutWidget.generated.h"

class URogueActionSystemComponent;
class UUserWidget;

UCLASS(Abstract)
class ACTIONROGUELIKE_API URogueGameplayLayoutWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUserWidget> ActionWidgetContainer;
	
public:
	void RebindActionSystem(URogueActionSystemComponent* ASC);
};
