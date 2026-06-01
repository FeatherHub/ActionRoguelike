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
	void SyncToAttachedActorPosition();
	
public:
	
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeWorldWidget();
	
	UFUNCTION(BlueprintCallable, Category=WorldWidget)
	void TickWorldWidget();
	
	UPROPERTY(EditDefaultsOnly, Category=WorldWidget)
	FVector ActorOffSet;
	
	UPROPERTY(BlueprintReadWrite, Category=WorldWidget, meta=(ExposeOnSpawn=true))
	AActor* AttachedActor;
};
