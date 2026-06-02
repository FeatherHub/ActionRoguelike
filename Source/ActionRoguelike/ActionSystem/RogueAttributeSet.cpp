#include "RogueAttributeSet.h"

#include "RogueActionSystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


//////////////////////
// URogueAttributeSet
//////////////////////
URogueActionSystemComponent* URogueAttributeSet::GetOwningComponent() const
{
	return Cast<URogueActionSystemComponent>(GetOuter());
}

////////////////////////////////
// URogueHealthAttributeSet
///////////////////////////////
URogueHealthAttributeSet::URogueHealthAttributeSet()
{
	Health = FRogueAttribute(100.f, 0.f);
	HealthMax = Health;
}


void URogueHealthAttributeSet::PostApplyChange()
{
	Super::PostApplyChange();
	
	Health.Base = FMath::Clamp(Health.Base, 0.f, HealthMax.Base);  
}

void URogueHealthAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(URogueHealthAttributeSet, Health);
	DOREPLIFETIME(URogueHealthAttributeSet, HealthMax);
}

////////////////////////////////
// URogueCharacterAttributeSet
///////////////////////////////


void URogueCharacterAttributeSet::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	ApplyMoveSpeed();	
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

ACharacter* URogueCharacterAttributeSet::GetOwningCharacter() const
{
	URogueActionSystemComponent* ASC = GetOwningComponent();
	
	return Cast<ACharacter>(ASC->GetOuter()); 
}

////////////////////////////////
// URoguePlayerAttributeSet
///////////////////////////////

URoguePlayerAttributeSet::URoguePlayerAttributeSet()
{
	MoveSpeed = FRogueAttribute(550.f, 0.f, 1.f);
}

////////////////////////////////
// URogueMonsterAttributeSet
///////////////////////////////

URogueMonsterAttributeSet::URogueMonsterAttributeSet()
{
	MoveSpeed = FRogueAttribute(450.f, 0.f, 1.f);
}



