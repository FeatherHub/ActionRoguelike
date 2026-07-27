#include "RoguePlayerController.h"

#include "EnhancedInputComponent.h"
#include "RogueInteractionComponent.h"
#include "Blueprint/UserWidget.h"


ARoguePlayerController::ARoguePlayerController()
{
	InteractionComp = CreateDefaultSubobject<URogueInteractionComponent>(TEXT("InteractionComp"));
}

void ARoguePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);

	EIC->BindAction(IA_Interact, ETriggerEvent::Triggered, this, &ThisClass::Interact);
	EIC->BindAction(IA_ToggleInGameMenu, ETriggerEvent::Triggered, this, &ThisClass::ToggleInGameMenu);
}

void ARoguePlayerController::Interact()
{
	APawn* PossessedPawn = GetPawn();
	if(IsValid(PossessedPawn))
	{
		InteractionComp->Interact(PossessedPawn);
	}
}

void ARoguePlayerController::ToggleInGameMenu()
{
	if (InGameMenuWidget)
	{
		SetShowMouseCursor(false);

		InGameMenuWidget->RemoveFromParent();
		InGameMenuWidget = nullptr;

		if (GetPawn())
		{
			GetPawn()->EnableInput(this);
		}
	}
	else
	{
		if (GetPawn())
		{
			GetPawn()->DisableInput(this);
		}

		InGameMenuWidget = CreateWidget<UUserWidget>(this, InGameMenuWidgetClass);
		InGameMenuWidget->AddToViewport();
		
		SetShowMouseCursor(true);
	}
}
