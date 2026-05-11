#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RogueAICharacter.generated.h"

class URogueActionSystemComponent;

UCLASS()
class ACTIONROGUELIKE_API ARogueAICharacter : public ACharacter
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, Category=Action)
	TObjectPtr<URogueActionSystemComponent> ActionSystemComp;
	
public:
	ARogueAICharacter();
	virtual void PostInitializeComponents() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
protected:
	FTimerHandle TimerHandle_FitFlashOverlay;
};
