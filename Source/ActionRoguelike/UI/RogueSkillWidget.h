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
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ActionIconImage;

	UPROPERTY(EditAnywhere, Category=Action)
	TSubclassOf<URogueActionBase> ActionClass;

	UPROPERTY(VisibleDefaultsOnly, Category="Material Contract")
	FName ActionIconTextureParamName = TEXT("ActionIconTexture");

	UPROPERTY(VisibleDefaultsOnly, Category="Material Contract")
	FName CooldownProgressParamName = TEXT("CooldownProgress");
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ActionIconMID;
	
	TWeakObjectPtr<URogueActionBase> BoundAction;
	TWeakObjectPtr<URogueActionSystemComponent> BoundASC;
	FGameplayTag ActionName;
	float LastCooldownProgress = -1.f;
	
public:
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void BindActionSystem(URogueActionSystemComponent* ASC);
	
protected:
	void ApplyStaticActionData();
#if WITH_EDITOR
	void ValidateAssetSetup() const;
#endif
};
