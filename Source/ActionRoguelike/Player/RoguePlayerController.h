#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RoguePlayerController.generated.h"

class UInputAction;
class URogueInteractionComponent;
class UUserWidget;

UCLASS()
class ACTIONROGUELIKE_API ARoguePlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category=Component)
	TObjectPtr<URogueInteractionComponent> InteractionComp;
	
	UPROPERTY(EditDefaultsOnly, Category=Input)
	TObjectPtr<UInputAction> IA_Interact;
	
	UPROPERTY(EditDefaultsOnly, Category=Input)
	TObjectPtr<UInputAction> IA_ToggleInGameMenu;
	
	void Interact();
	
	UFUNCTION(BlueprintCallable)
	void ToggleInGameMenu();
	
	UPROPERTY(EditDefaultsOnly, Category=Menu)
	TSubclassOf<UUserWidget> InGameMenuWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> InGameMenuWidget;
	
public:
	ARoguePlayerController();
	virtual void SetupInputComponent() override;
};
