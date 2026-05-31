#include "RogueWorldWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"

inline TAutoConsoleVariable<bool> CVarWorldWidgetDrawDebug{TEXT("rogue.worldwidget.DebugDraw"), true, 
	TEXT("Draw World Widget info. (0=off. 1=on)"), ECVF_Cheat};

void URogueWorldWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetLocation();
}

void URogueWorldWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (CVarWorldWidgetDrawDebug.GetValueOnGameThread())
	{
		FColor DebugColor = AttachedActor ? FColor::Green : FColor::Red;
		FString DebugMessage = AttachedActor ? FString::Printf(TEXT("Attached Actor: %s"), *GetNameSafe(AttachedActor)) : "No Attached Actor";
		DrawDebugSphere(GetWorld(), AttachedActor->GetActorLocation(), 32.f, 24, FColor::Blue);
		GEngine->AddOnScreenDebugMessage(20, 0.f, DebugColor, DebugMessage);
	}
	
	if (!IsValid(AttachedActor))
	{
		RemoveFromParent();
		
		return;
	}
	
	SetLocation();
}

void URogueWorldWidget::SetLocation()
{
	FVector2D ScreenPosition;
	if (UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachedActor->GetActorLocation(), ScreenPosition))
	{
		SetPositionInViewport(ScreenPosition, true);
	}
	if (CVarWorldWidgetDrawDebug.GetValueOnGameThread())
	{
		FString ProjectionInfo = FString::Printf(TEXT("Projected From World %s to Screen %s"), \
			*AttachedActor->GetActorLocation().ToString(),  *ScreenPosition.ToString()
		);
		
		GEngine->AddOnScreenDebugMessage(21, 0.f, FColor::Blue, ProjectionInfo);
	}
}
