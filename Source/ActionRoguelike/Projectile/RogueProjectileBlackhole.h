// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RogueProjectileBase.h"
#include "GameFramework/Actor.h"
#include "RogueProjectileBlackhole.generated.h"

class URadialForceComponent;

UCLASS(Abstract)
class ACTIONROGUELIKE_API ARogueProjectileBlackhole : public ARogueProjectileBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category=Attack)
	TObjectPtr<URadialForceComponent> BlackholeRadialForceComp;
	
	UPROPERTY(EditDefaultsOnly, Category=Attack)
	float BlackholeLifeTime;
	
protected:
	virtual void BeginPlay() override;
	virtual void Explode() override;
	
public:
	ARogueProjectileBlackhole();
	virtual void PostInitializeComponents() override;
	
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
