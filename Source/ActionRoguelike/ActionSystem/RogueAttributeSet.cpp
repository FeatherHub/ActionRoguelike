#include "RogueAttributeSet.h"

#include "RogueActionSystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


URogueHealthAttributeSet::URogueHealthAttributeSet()
{
	Health = FRogueAttribute(100.f, 0.f);
	HealthMax = Health;
}

URoguePlayerAttributeSet::URoguePlayerAttributeSet()
{
	MoveSpeed = FRogueAttribute(550.f, 0.f, 1.f);
}

URogueMonsterAttributeSet::URogueMonsterAttributeSet()
{
	MoveSpeed = FRogueAttribute(450.f, 0.f, 1.f);
}

void URogueCharacterAttributeSet::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	ApplyMoveSpeed();	
}

void URogueHealthAttributeSet::PostApplyChange()
{
	Super::PostApplyChange();
	
	Health.Base = FMath::Clamp(Health.Base, 0.f, HealthMax.Base);  
}

void URogueCharacterAttributeSet::PostApplyChange()
{
	Super::PostApplyChange();

	ApplyMoveSpeed();
}

void URogueCharacterAttributeSet::ApplyMoveSpeed()
{
	UCharacterMovementComponent* CMC= Cast<UCharacterMovementComponent>(GetOwningCharacter()->GetMovementComponent());
	if (CMC)
	{
		CMC->MaxWalkSpeed = MoveSpeed.GetValue();
	}
}

URogueActionSystemComponent* URogueAttributeSet::GetOwningComponent() const
{
	return Cast<URogueActionSystemComponent>(GetOuter());
}

ACharacter* URogueCharacterAttributeSet::GetOwningCharacter() const
{
	URogueActionSystemComponent* ASC = GetOwningComponent();
	
	return Cast<ACharacter>(ASC->GetOuter()); 
}
