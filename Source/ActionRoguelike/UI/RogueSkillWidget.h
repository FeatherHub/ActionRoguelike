#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "RogueSkillWidget.generated.h"

class URogueActionSystemComponent;
class URogueActionBase;
class UImage;

UCLASS(Abstract)
class ACTIONROGUELIKE_API URogueSkillWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category=Action)
	TSubclassOf<URogueActionBase> ActionClass;
	FGameplayTag ActionName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ActionIconImage;
	
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> ActionIconMID;
	
public:
	void BindActionSystem(URogueActionSystemComponent* ASC);
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;	
};
