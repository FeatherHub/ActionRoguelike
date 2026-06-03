#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RogueExplosiveBarrel.generated.h"


class UNiagaraComponent;
class URadialForceComponent;
class UNiagaraSystem;
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class ACTIONROGUELIKE_API ARogueExplosiveBarrel : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, Category=Component)
	TObjectPtr<UStaticMeshComponent> SMComp;
	
	UPROPERTY(VisibleAnywhere, Category=Component)
	TObjectPtr<UBoxComponent> BoxShapeComp;
	
	UPROPERTY(VisibleAnywhere, Category=Component)
	TObjectPtr<URadialForceComponent> ExplodeRadialForceComp;
	
	UPROPERTY(EditDefaultsOnly, Category=Effect)
	TObjectPtr<UNiagaraSystem> FuseVFX;

	UPROPERTY(EditDefaultsOnly, Category=Effect)
	TObjectPtr<USoundBase> FuseSFX;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> SpawnedFuseVFX;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> SpawnedFuseSFX;

	UPROPERTY(EditDefaultsOnly, Category=Effect)
	TObjectPtr<UNiagaraSystem> ExplodeVFX;

	UPROPERTY(EditDefaultsOnly, Category=Effect)
	TObjectPtr<USoundBase> ExplodeSFX;
	
	UPROPERTY(EditDefaultsOnly, Category=Damage)
	TSubclassOf<UDamageType> DmgTypeClass;
	
	UPROPERTY(EditDefaultsOnly, Category=Damage)
	float ExplodeDelay;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastStartExplodeSequence();
	
	UFUNCTION()
	void Fuse();
	
	UFUNCTION()
	void Explode();
	
public:
	ARogueExplosiveBarrel();
	virtual void PostInitializeComponents() override;
};
