#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RogueWorldWidget.generated.h"


class USizeBox;

UCLASS()
class ACTIONROGUELIKE_API URogueWorldWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta=(BindWidget))
	USizeBox* ParentSizeBox;
	
	UFUNCTION(BlueprintCallable, Category=WorldWidget)
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SyncToAttachedActorPosition();

public:
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeWorldWidget();

	UPROPERTY(EditDefaultsOnly, Category=WorldWidget)
	FVector ActorOffSet;
	
	UPROPERTY(BlueprintReadWrite, Category=WorldWidget, meta=(ExposeOnSpawn=true))
	AActor* AttachedActor;
};
