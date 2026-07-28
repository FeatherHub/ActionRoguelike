#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "RogueToastChannelWidget.generated.h"


class UTextBlock;
struct FRogueToastMessage;
class URogueToastMessageSubsystem;


UCLASS(Abstract)
class ACTIONROGUELIKE_API URogueToastChannelWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> BodyTextWidget;
	
	UPROPERTY(EditAnywhere, Category=Toast)
	FGameplayTag ChannelTag;

	TWeakObjectPtr<URogueToastMessageSubsystem> BoundSystem;

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
protected:
	void OnToastMessagePushed(const FRogueToastMessage& Message);
	
};
