#pragma once

#include "CoreMinimal.h"
#include "Core/RogueInteractionInterface.h"
#include "GameFramework/Actor.h"
#include "RogueItemChest.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ARogueItemChest : public AActor, public IRogueInteractionInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Component)
	TObjectPtr<UStaticMeshComponent> LidMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Component)
	TObjectPtr<UStaticMeshComponent> BaseMeshComp;

	UPROPERTY(ReplicatedUsing=OnRep_IsLidOpen, BlueprintReadOnly, Category=Chest)
	bool bIsLidOpen;
	
	UFUNCTION()
	void OnRep_IsLidOpen();
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnIsLidOpenChanged(bool bNewIsLidOpen);

public:
	ARogueItemChest();
	virtual void Interact_Implementation() override;
};
