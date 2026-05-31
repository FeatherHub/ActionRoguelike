#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RogueInteractionComponent.generated.h"


class URogueWorldWidget;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROGUELIKE_API URogueInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category=Interaction)
	float InteractionRadius;

	UPROPERTY(EditDefaultsOnly, Category=Interaction)
	float DistanceWeightScale;

	UPROPERTY(EditDefaultsOnly, Category=Interaction)
	float DirectionWeightScale;

	UPROPERTY(EditDefaultsOnly, Category=Interaction)
	TSubclassOf<URogueWorldWidget> InteractionPromptWidgetClass;

	UPROPERTY(VisibleInstanceOnly, Category=Interaction)
	URogueWorldWidget* InteractionPromptWidget;
	
	UPROPERTY(VisibleInstanceOnly, Category=Interaction)
	AActor* InteractableActor;
	
	AActor* FindInteractableActor();
	
public:
	URogueInteractionComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void Interact();
};
