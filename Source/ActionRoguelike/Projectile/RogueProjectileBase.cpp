#include "RogueProjectileBase.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Core/RogueDebug.h"
#include "Core/RogueGameType.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


ARogueProjectileBase::ARogueProjectileBase()
{
	bReplicates = true;
	
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	InFlightNiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("InFlightNiagaraComp"));
	InFlightAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("InFlightAudioComp"));
	
	RootComponent = SphereComp;

	SphereComp->SetCollisionProfileName(RogueCollision::Profile::Projectile);
	
	InFlightAudioComp->SetupAttachment(SphereComp);
	InFlightNiagaraComp->SetupAttachment(SphereComp);
	
	MovementComp->ProjectileGravityScale = 0.f;
	
	InitialLifeSpan = 30.f;
}

void ARogueProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
}

void ARogueProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::PlaySoundAtLocation(this, SpawnSoundEffect, GetActorLocation(), GetActorRotation());
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SpawnNiagaraEffect, GetActorLocation(), GetActorRotation());
}


void ARogueProjectileBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
#if !UE_BUILD_SHIPPING
	float DebugDrawDuration = CVarProjectileDrawDebug.GetValueOnGameThread();
	
	if (DebugDrawDuration > 0.f)
	{
		DrawDebugSphere(GetWorld(), Hit.Location, SphereComp->GetScaledSphereRadius(), 16.f, FColor::Red, false, DebugDrawDuration);
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, SphereComp->GetScaledSphereRadius(), 18.f, FColor::Orange, false, DebugDrawDuration);
	}
#endif
}

void ARogueProjectileBase::Explode()
{
	UGameplayStatics::PlaySoundAtLocation(this, ExplosionSoundEffect, GetActorLocation(), GetActorRotation());
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionNiagaraEffect, GetActorLocation(), GetActorRotation());
}
