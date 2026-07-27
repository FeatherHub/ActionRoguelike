#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RogueGameplayHUD.generated.h"

class URogueGameplayLayoutWidget;

UCLASS(Abstract)
class ACTIONROGUELIKE_API ARogueGameplayHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<URogueGameplayLayoutWidget> GameplayLayoutWidgetClass;
	
	UPROPERTY(Transient)
	TObjectPtr<URogueGameplayLayoutWidget> GameplayLayoutWidget;
	
protected:
	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	void RefreshActionSystemBinding();
	
public:
	virtual void BeginPlay() override;
	
};
