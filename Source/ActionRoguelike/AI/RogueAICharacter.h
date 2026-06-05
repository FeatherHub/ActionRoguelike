#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RogueAICharacter.generated.h"

class URogueWorldWidget;
class URogueActionSystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathStart);

UCLASS()
class ACTIONROGUELIKE_API ARogueAICharacter : public ACharacter
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Action)
	TObjectPtr<URogueActionSystemComponent> ActionSystemComp;
	
	UPROPERTY(EditDefaultsOnly, NoClear, Category=Animation)
	TObjectPtr<UAnimMontage> AnimMontage_Death;
	
	UPROPERTY(EditDefaultsOnly, Category=Widget)
	TSubclassOf<URogueWorldWidget> HealthWorldWidgetClass;
	
	UPROPERTY()
	TObjectPtr<URogueWorldWidget> HealthWorldWidget;
	
public:
	ARogueAICharacter();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	FOnDeathStart OnDeathStart;
protected:
	FTimerHandle TimerHandle_FitFlashOverlay;
};
