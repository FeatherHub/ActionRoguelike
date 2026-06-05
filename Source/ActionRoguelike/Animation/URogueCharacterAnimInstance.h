#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "URogueCharacterAnimInstance.generated.h"

struct FGameplayTag;
class URogueActionSystemComponent;

UCLASS()
class ACTIONROGUELIKE_API UURogueCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<URogueActionSystemComponent> ActionSystemComp;

	UPROPERTY(Transient, BlueprintReadonly)
	bool bIsSprinting;
	
	UPROPERTY(Transient, BlueprintReadonly)
	bool bIsStunned;
	
	UFUNCTION()
	void OnGameplayTagUpdated(FGameplayTag UpdatedTag, int32 NewCount);
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;
};
