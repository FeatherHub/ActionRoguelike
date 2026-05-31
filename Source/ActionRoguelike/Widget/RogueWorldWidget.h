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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void SetLocation();
	
public:
	UPROPERTY(EditDefaultsOnly, Category=WorldUI)
	FVector WorldOffset;
	
	UPROPERTY(BlueprintReadWrite, Category=WorldUI, meta=(ExposeOnSpawn=true))
	AActor* AttachedActor;
};
