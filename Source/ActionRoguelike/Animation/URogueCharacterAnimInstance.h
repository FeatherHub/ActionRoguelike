#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "URogueCharacterAnimInstance.generated.h"

class URogueActionSystemComponent;

UCLASS()
class ACTIONROGUELIKE_API UURogueCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(Transient, BlueprintReadonly)
	bool bIsSprinting;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<URogueActionSystemComponent> ActionSystemComp;
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
