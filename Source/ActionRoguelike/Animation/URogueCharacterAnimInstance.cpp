// Fill out your copyright notice in the Description page of Project Settings.


#include "URogueCharacterAnimInstance.h"

#include "ActionSystem/RogueActionSystemComponent.h"
#include "Core/RogueGameplayTag.h"

void UURogueCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	ActionSystemComp = GetOwningActor()->FindComponentByClass<URogueActionSystemComponent>();
}

void UURogueCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (ActionSystemComp)
	{
		 bIsSprinting = ActionSystemComp->ActiveTags.HasTag(RogueGameplayTag::Status_Sprinting);
	}
}
