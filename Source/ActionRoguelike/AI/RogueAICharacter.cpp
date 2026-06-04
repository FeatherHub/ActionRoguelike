#include "RogueAICharacter.h"

#include "ActionSystem/RogueActionSystemComponent.h"
#include "ActionSystem/RogueAttributeSet.h"
#include "Core/RogueGameplayTag.h"
#include "Widget/RogueWorldWidget.h"


ARogueAICharacter::ARogueAICharacter()
{
	ActionSystemComp = CreateDefaultSubobject<URogueActionSystemComponent>(TEXT("ActionSystemComp"));
	ActionSystemComp->SetDefaultAttributeSet(URogueMonsterAttributeSet::StaticClass());
}

void ARogueAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	GetMesh()->SetOverlayMaterialMaxDrawDistance(1.f);
}

void ARogueAICharacter::BeginPlay()
{
	Super::BeginPlay();

	if(ensure(HealthWorldWidgetClass))
	{
		HealthWorldWidget = CreateWidget<URogueWorldWidget>(GetWorld(), HealthWorldWidgetClass);
		HealthWorldWidget->AttachedActor = this;
		HealthWorldWidget->AddToViewport();
	}
}

float ARogueAICharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ActionSystemComp->ApplyAttributeChange(RogueGameplayTag::Attribute_Health, -ActualDamageAmount, BaseDelta);

	// Hit Flash
	GetMesh()->SetOverlayMaterialMaxDrawDistance(0.f);
	GetMesh()->SetCustomPrimitiveDataFloat(0, GetWorld()->TimeSeconds);
	GetWorldTimerManager().SetTimer(TimerHandle_FitFlashOverlay, [this]()
	{
		if (IsValid(this))
		{
			GetMesh()->SetOverlayMaterialMaxDrawDistance(1.f);
		}
	}, 1.f, false);

	// Handle Death
	float NewHealth = ActionSystemComp->GetAttributeValue(RogueGameplayTag::Attribute_Health);
	if (NewHealth < 0.f)
	{
		if(HealthWorldWidget)
		{
			HealthWorldWidget->RemoveFromParent();
			SetLifeSpan(3.f);
		}
	}
	
	return ActualDamageAmount;
}
