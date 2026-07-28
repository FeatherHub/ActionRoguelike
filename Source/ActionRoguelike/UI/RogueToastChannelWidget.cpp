#include "RogueToastChannelWidget.h"

#include "Components/TextBlock.h"
#include "ToastMessageSystem/RogueToastMessage.h"
#include "ToastMessageSystem/RogueToastMessageSubsystem.h"


void URogueToastChannelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
	if(!LocalPlayer)
	{
		return;
	}
	
	BoundSystem = ULocalPlayer::GetSubsystem<URogueToastMessageSubsystem>(LocalPlayer);
	if(!BoundSystem.IsValid())
	{
		return;
	}
	
	BoundSystem->OnToastMessagePushed.AddUObject(this, &ThisClass::OnToastMessagePushed);
}


void URogueToastChannelWidget::NativeDestruct()
{
	if(URogueToastMessageSubsystem* ToastSystem = BoundSystem.Get())
	{
		ToastSystem->OnToastMessagePushed.RemoveAll(this);
	}
	BoundSystem.Reset();

	Super::NativeDestruct();
}


void URogueToastChannelWidget::OnToastMessagePushed(const FRogueToastMessage& Message)
{
	if (Message.ChannelTag.MatchesTag(ChannelTag))
	{
		BodyTextWidget->SetText(Message.Body);	
	}
}
