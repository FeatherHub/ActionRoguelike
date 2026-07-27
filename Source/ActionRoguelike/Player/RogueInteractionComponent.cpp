#include "RogueInteractionComponent.h"

#include "Core/RogueGameType.h"
#include "Core/RogueInteractionInterface.h"
#include "Engine/OverlapResult.h"
#include "Components/PanelWidget.h"
#include "DebugSystem/DebugUtil.h"
#include "Network/NetUtil.h"
#include "UI/RogueWorldWidget.h"

static TAutoConsoleVariable<bool> CVarInteractionDebugDraw{
	TEXT("rogue.interaction.Debugdraw"), false,
	TEXT("Enable interation debug draw. (0 = Off, 1 = On)"), ECVF_Default
};

const float DEBUG_BOX_EXTENT = 50.f;

URogueInteractionComponent::URogueInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	InteractionRadius = 800.f;
	DistanceWeightScale = 2.f;
	DirectionWeightScale = 1.f;
}

void URogueInteractionComponent::Interact(APawn* InstigatorPawn)
{
	DEBUG_ONSCREEN_CVARFMT(CVarInteractionDebugDraw, 0, 3.f, FColor::Blue, 
		TEXT("[Interact] InteractableActor: %s"), *GetNameSafe(InteractableActor));
	
	Interact_Server(InteractableActor, InstigatorPawn);
}

void URogueInteractionComponent::Interact_Server_Implementation(AActor* ActorToInteract, APawn* InstigatorPawn)
{
	DEBUG_ONSCREEN_CVARFMT(CVarInteractionDebugDraw, 0, 3.f, FColor::Green,
		TEXT("[InteractServer] ActorToInteract: %s"), *GetNameSafe(ActorToInteract));

	if (ActorToInteract && ActorToInteract->Implements<URogueInteractionInterface>())
	{
		IRogueInteractionInterface::Execute_Interact(ActorToInteract, InstigatorPawn);
	}
}


void URogueInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APlayerController* OwningPC = Cast<APlayerController>(GetOwner());

	DEBUG_ONSCREEN_CVARFMT(CVarInteractionDebugDraw, 0, 0.f, FColor::White,
		TEXT("[InteractionComp::Tick] %s Is Local Controller? %d"),
			*NetUtil::GetNetName(OwningPC), OwningPC->IsLocalController());
	
	if (!OwningPC->IsLocalController())
	{
		return;
	}
	
	InteractableActor = FindInteractableActor();
	
	if (InteractableActor)
	{
		if (!IsValid(InteractionPromptWidget) && ensure(InteractionPromptWidgetClass))
		{
			InteractionPromptWidget = CreateWidget<URogueWorldWidget>(Cast<APlayerController>(GetOwner()), InteractionPromptWidgetClass);
			InteractionPromptWidget->AttachedActor = InteractableActor;
			InteractionPromptWidget->InitializeWorldWidget();
			InteractionPromptWidget->AddToViewport();
		}
		
		if (IsValid(InteractionPromptWidget))
		{
			InteractionPromptWidget->AttachedActor = InteractableActor;
		}
	}
	else
	{
		if (IsValid(InteractionPromptWidget))
		{
			InteractionPromptWidget->RemoveFromParent();
			InteractionPromptWidget = nullptr;
		}
	}
	
	DEBUG_IF_CVAR(CVarInteractionDebugDraw)
	{
		FString InteractableActorMsg = InteractableActor  
			? FString::Printf(TEXT("[InteractComp::Tick] Interactable Actor: %s"), *GetNameSafe(InteractableActor))  
			: "[InteractComp::Tick] No Interactable Actor";
		
		DEBUG_ONSCREEN(0, 0.f, FColor::White, InteractableActorMsg);

		FString PromptWidgetMsg = InteractionPromptWidget
			? FString::Printf(TEXT("[InteractComp::Tick] Widget In Viewport? %d Enabled? %d "),InteractionPromptWidget->IsInViewport(), InteractionPromptWidget->GetIsEnabled())
			: "[InteractComp::Tick] No Prompt Widget";

		DEBUG_ONSCREEN(0, 0.f, FColor::White, PromptWidgetMsg);
	}
}

AActor* URogueInteractionComponent::FindInteractableActor()
{
	APlayerController* PC = CastChecked<APlayerController>(GetOwner());
	APawn* PlayerPawn = PC->GetPawn();
	if (!IsValid(PlayerPawn))
	{
		return nullptr;
	}
	
	FVector ControllerDirection = PC->GetControlRotation().Vector();
	FVector EyeLocation = PC->PlayerCameraManager->GetCameraLocation();
	float MaxDistanceSqrd = InteractionRadius * InteractionRadius;

	TArray<FOverlapResult> Overlaps;
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(InteractionRadius);
	GetWorld()->OverlapMultiByChannel(Overlaps, EyeLocation, FQuat::Identity, RogueCollision::Trace::Interaction, CollisionShape);

	// Weight range is [0,1]
	float HighestWeight = 0;
	AActor* BestCandidate = nullptr;

	for (FOverlapResult& Overlap : Overlaps)
	{
		FVector OverlapLocation;
		FVector OverlapExtent;
		Overlap.GetActor()->GetActorBounds(true, OverlapLocation, OverlapExtent);

		float Distance = FVector::DistSquared(PlayerPawn->GetActorLocation(), OverlapLocation);

		// normalize to [0,1]. closer distance gets the higher value
		float NormalizedDistance = 1.f - (Distance / MaxDistanceSqrd);

		FVector TargetDirection = (OverlapLocation - EyeLocation).GetSafeNormal();
		float Dot = FVector::DotProduct(ControllerDirection, TargetDirection);

		// normalize to [0,1]
		float NormalizedDot = Dot * 0.5f + 0.5f;

		float Weight = (NormalizedDot * DirectionWeightScale) + (NormalizedDistance * DistanceWeightScale);

		if (Weight > HighestWeight)
		{
			HighestWeight = Weight;
			BestCandidate = Overlap.GetActor();
		}
		
		DEBUG_IF_CVAR (CVarInteractionDebugDraw)
		{
			DrawDebugBox(GetWorld(), OverlapLocation, FVector{DEBUG_BOX_EXTENT}, FColor::Red, false);
			
			DrawDebugString(GetWorld(), OverlapLocation, FString::Printf(TEXT("Weight: %f Dot: %f Dist: %f"), Weight, NormalizedDot, NormalizedDistance), 
				nullptr, FColor::White, 0, true);
		}
	}

	DEBUG_IF_CVAR (CVarInteractionDebugDraw)
	{
		if (InteractableActor)
		{
			FVector Origin;
			FVector Extent;
			InteractableActor->GetActorBounds(true, Origin, Extent);
			DrawDebugBox(GetWorld(), Origin, FVector{DEBUG_BOX_EXTENT + 10.f}, FColor::Green, false);
		}

		DrawDebugSphere(GetWorld(), EyeLocation, InteractionRadius, 16.f, FColor::White, false);
	}
	
	return BestCandidate;
}
