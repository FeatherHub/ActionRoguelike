#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RogueWorldWidget.generated.h"


UCLASS()
class ACTIONROGUELIKE_API URogueWorldWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	void UpdateScreenPosition();
	
public:
	UFUNCTION(BlueprintCallable, Category=WorldWidget)
	void TickWorldWidget();
	
	UPROPERTY(EditDefaultsOnly, Category=WorldWidget)
	FVector WorldOffset;
	
	UPROPERTY(BlueprintReadWrite, Category=WorldWidget, meta=(ExposeOnSpawn=true))
	AActor* AttachedActor;
};
