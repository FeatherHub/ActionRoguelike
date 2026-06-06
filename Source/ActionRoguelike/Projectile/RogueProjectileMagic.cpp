#include "RogueProjectileMagic.h"

#include "ActionSystem/RogueActionSystemComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ActionSystem/RogueActionEffect.h"

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
	
	UGameplayStatics::ApplyPointDamage(OtherActor, DamageAmount, HitFromDirection, Hit, GetInstigatorController(), this, DamageTypeClass);
	
	// note: awkward couping; low level projectile knows high level ActionSystem
	if(URogueActionSystemComponent* ASC = OtherActor->GetComponentByClass<URogueActionSystemComponent>())
	{
		ASC->GrantAction(HitEffect);
	}
	
	Destroy();
}