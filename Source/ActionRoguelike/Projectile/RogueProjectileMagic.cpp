#include "RogueProjectileMagic.h"

#include "Components/SphereComponent.h"
#include "ActionSystem/RogueGameplayStatics.h"

ARogueProjectileMagic::ARogueProjectileMagic()
{
	InitialLifeSpan = 8.f;
	DamageAmount = 10.f;
}

void ARogueProjectileMagic::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereComp->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
}

void ARogueProjectileMagic::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
	
	Explode();
	
	FVector HitFromDirection = GetActorRotation().Vector();
	
	FRogueGameplayStatics::ApplyPointDamage(OtherActor, DamageAmount, HitFromDirection, Hit, GetInstigatorController(), this, DamageTypeClass, ActionEffectOnHit);	
	
	Destroy();
}