#include "RogueWorldWidget.h"

#include "Kismet/GameplayStatics.h"

inline TAutoConsoleVariable<bool> CVarWorldWidgetDrawDebug{TEXT("rogue.worldwidget.DebugDraw"), false, 
	TEXT("Draw World Widget info. (0=off. 1=on)"), ECVF_Cheat};


void URogueWorldWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetAlignmentInViewport({0.5f, 0.5f});
}

void URogueWorldWidget::TickWorldWidget()
{
#if !UE_BUILD_SHIPPING
	if (CVarWorldWidgetDrawDebug.GetValueOnGameThread())
	{
		FColor DebugColor = AttachedActor ? FColor::Green : FColor::Red;
		FString AttachedActorMsg = AttachedActor ? FString::Printf(TEXT("[World Widget] Attached Actor: %s"), *GetNameSafe(AttachedActor)) : "[World Widget] No Attached Actor";

		DrawDebugSphere(GetWorld(), AttachedActor->GetActorLocation(), 32.f, 24, FColor::Blue);
		GEngine->AddOnScreenDebugMessage(20, 0.f, DebugColor, AttachedActorMsg);
	}
#endif
	
	if (!IsValid(AttachedActor))
	{
		RemoveFromParent();
		
		return;
	}
	
	SyncToAttachedActorPosition();
}

void URogueWorldWidget::SyncToAttachedActorPosition()
{
	FVector2D ScreenPosition;
	FVector WorldPosition = AttachedActor->GetActorLocation() + ActorOffSet;
	
	bool bIsInFrontOfCamera = UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), WorldPosition, ScreenPosition); 
	if (bIsInFrontOfCamera)
	{
		SetPositionInViewport(ScreenPosition, true);
	}
	
#if !UE_BUILD_SHIPPING
	if (CVarWorldWidgetDrawDebug.GetValueOnGameThread())
	{
		FColor DebugColor = bIsInFrontOfCamera ? FColor::Blue : FColor::Red;
		FString ProjectionMsg = TEXT("[WorldWiget] Is not in front of camera");
		if (bIsInFrontOfCamera)
		{
			ProjectionMsg = FString::Printf(TEXT("[WorldWiget] Is in front of camera. Projected From World %s to Screen %s"),
				*AttachedActor->GetActorLocation().ToString(),  *ScreenPosition.ToString()); 
		}
		
		GEngine->AddOnScreenDebugMessage(21, 0.f, DebugColor, ProjectionMsg);
	}
#endif
}
