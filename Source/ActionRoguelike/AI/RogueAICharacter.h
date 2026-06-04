#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RogueAICharacter.generated.h"

class URogueWorldWidget;
class URogueActionSystemComponent;

UCLASS()
class ACTIONROGUELIKE_API ARogueAICharacter : public ACharacter
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Action)
	TObjectPtr<URogueActionSystemComponent> ActionSystemComp;
	
	UPROPERTY(EditDefaultsOnly, Category=Widget)
	TSubclassOf<URogueWorldWidget> HealthWorldWidgetClass;
	
	UPROPERTY()
	TObjectPtr<URogueWorldWidget> HealthWorldWidget;
	
public:
	ARogueAICharacter();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
protected:
	FTimerHandle TimerHandle_FitFlashOverlay;
};
