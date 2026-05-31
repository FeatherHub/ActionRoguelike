// Fill out your copyright notice in the Description page of Project Settings.


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
	InteractionComp->Interact();
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
