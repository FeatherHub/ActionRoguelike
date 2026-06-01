#include "RogueWorldWidget.h"

#include "Kismet/GameplayStatics.h"

inline TAutoConsoleVariable<bool> CVarWorldWidgetDrawDebug{TEXT("rogue.worldwidget.DebugDraw"), true, 
	TEXT("Draw World Widget info. (0=off. 1=on)"), ECVF_Cheat};

void URogueWorldWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	UpdateScreenPosition();
}

void URogueWorldWidget::TickWorldWidget()
{
#if !UE_BUILD_SHIPPING
	if (CVarWorldWidgetDrawDebug.GetValueOnGameThread())
	{
		FColor DebugColor = AttachedActor ? FColor::Green : FColor::Red;
		FString DebugMessage = AttachedActor ? FString::Printf(TEXT("Attached Actor: %s"), *GetNameSafe(AttachedActor)) : "No Attached Actor";

		DrawDebugSphere(GetWorld(), AttachedActor->GetActorLocation(), 32.f, 24, FColor::Blue);
		GEngine->AddOnScreenDebugMessage(20, 0.f, DebugColor, DebugMessage);
	}
#endif
	
	if (!IsValid(AttachedActor))
	{
		RemoveFromParent();
		
		return;
	}
	
	UpdateScreenPosition();
}

void URogueWorldWidget::UpdateScreenPosition()
{
	FVector2D ScreenPosition;
	if (UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachedActor->GetActorLocation(), ScreenPosition))
	{
		SetPositionInViewport(ScreenPosition, true);
	}
	
#if !UE_BUILD_SHIPPING
	if (CVarWorldWidgetDrawDebug.GetValueOnGameThread())
	{
		FString ProjectionMsg = FString::Printf(TEXT("Projected From World %s to Screen %s"), \
			*AttachedActor->GetActorLocation().ToString(),  *ScreenPosition.ToString()
		);
		
		GEngine->AddOnScreenDebugMessage(21, 0.f, FColor::Blue, ProjectionMsg);
	}
#endif
}

