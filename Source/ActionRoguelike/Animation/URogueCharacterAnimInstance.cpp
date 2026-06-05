#include "URogueCharacterAnimInstance.h"

#include "ActionSystem/RogueActionSystemComponent.h"
#include "Core/RogueGameplayTag.h"


void UURogueCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	ActionSystemComp = GetOwningActor()->FindComponentByClass<URogueActionSystemComponent>();
	
	ensureAlways(ActionSystemComp);
}

void UURogueCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	if(ActionSystemComp)
	{
		ActionSystemComp->OnGameplayTagUpdated.AddDynamic(this, &ThisClass::OnGameplayTagUpdated);
	}
}

void UURogueCharacterAnimInstance::OnGameplayTagUpdated(FGameplayTag UpdatedTag, int32 NewCount)
{
	if (!ActionSystemComp)
	{
		return;
	}	
	
	bool bWasAdded = NewCount > 0;
	if(UpdatedTag == RogueGameplayTag::Status_Sprinting)
	{
		bIsSprinting = bWasAdded;
	} else if(UpdatedTag == RogueGameplayTag::Status_Stunned)
	{
		bIsStunned = bWasAdded;
	}
}
