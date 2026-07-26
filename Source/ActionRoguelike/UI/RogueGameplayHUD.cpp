#include "RogueGameplayHUD.h"

#include "Blueprint/UserWidget.h"
#include "RogueGameplayLayoutWidget.h"
#include "ActionSystem/RogueActionSystemComponent.h"

void ARogueGameplayHUD::BeginPlay()
{
	Super::BeginPlay();
	
	GameplayLayoutWidget = CreateWidget<URogueGameplayLayoutWidget>(PlayerOwner, GameplayLayoutWidgetClass);
	GameplayLayoutWidget->AddToViewport();

	PlayerOwner->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::HandlePossessedPawnChanged);
}

void ARogueGameplayHUD::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	BindActionSystem();
}

void ARogueGameplayHUD::BindActionSystem()
{
	APawn* PlayerPawn = PlayerOwner->GetPawn();
	if(!IsValid(PlayerPawn))
	{
		return;
	}	
	
	URogueActionSystemComponent* ASC = PlayerPawn->FindComponentByClass<URogueActionSystemComponent>();
	if(!IsValid(ASC))
	{
		return;
	}

	GameplayLayoutWidget->BindActionSystem(ASC);
}
