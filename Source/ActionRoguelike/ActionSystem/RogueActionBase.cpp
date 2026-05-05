#include "RogueActionBase.h"

#include "RogueActionSystemComponent.h"
#include "GameFramework/Character.h"

void URogueActionBase::StartAction()
{
	float GameTime = GetWorld()->TimeSeconds;
	UE_LOGFMT(LogTemp, Log, "Start Action '{ActionName}' at {GameTime}", ActionName, GameTime);
}

URogueActionSystemComponent* URogueActionBase::GetOwningComponent() const
{
	return Cast<URogueActionSystemComponent>(GetOuter());
}

ACharacter* URogueActionBase::GetOwningCharacter() const
{
	return Cast<ACharacter>(GetOuter()->GetOuter());
}
