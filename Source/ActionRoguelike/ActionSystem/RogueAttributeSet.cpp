#include "RogueAttributeSet.h"

#include "RogueActionSystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


URogueHealthAttributeSet::URogueHealthAttributeSet()
{
	Health = FRogueAttribute(100.f, 0.f);
	HealthMax = Health;
}

URoguePlayerAttribute::URoguePlayerAttribute()
{
	MoveSpeed = FRogueAttribute(550.f, 0.f, 1.f);
}

URogueMonsterAttribute::URogueMonsterAttribute()
{
	MoveSpeed = FRogueAttribute(450.f, 0.f, 1.f);
}

void URogueCharacterAttribute::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	ApplyMoveSpeed();	
}

void URogueHealthAttributeSet::PostApplyChange()
{
	Super::PostApplyChange();
	
	Health.Base = FMath::Clamp(Health.Base, 0.f, HealthMax.Base);  
}

void URogueCharacterAttribute::PostApplyChange()
{
	Super::PostApplyChange();

	ApplyMoveSpeed();
}

void URogueCharacterAttribute::ApplyMoveSpeed()
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

ACharacter* URogueCharacterAttribute::GetOwningCharacter() const
{
	URogueActionSystemComponent* ASC = GetOwningComponent();
	
	return Cast<ACharacter>(ASC->GetOuter()); 
}
