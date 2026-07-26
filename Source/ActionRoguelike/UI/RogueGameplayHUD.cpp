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
	
	BindActionSystem(); // Standalone 경로
}

void ARogueGameplayHUD::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	BindActionSystem(); // Multiplayer Client 경로 or Pawn 변경 시 경로
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
