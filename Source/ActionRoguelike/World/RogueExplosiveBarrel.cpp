#include "RogueExplosiveBarrel.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "ActionRoguelike/Projectile/RogueProjectileMagic.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"


ARogueExplosiveBarrel::ARogueExplosiveBarrel()
{
	bReplicates = true;
	
	SMComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	BoxShapeComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxShapeComp"));
	RootComponent = BoxShapeComp;
	SMComp->SetupAttachment(BoxShapeComp);

	BoxShapeComp->SetSimulatePhysics(true);
	BoxShapeComp->SetEnableGravity(true);

	ExplodeRadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	ExplodeRadialForceComp->SetupAttachment(RootComponent);
	
	ExplodeDelay = 2.f;

	SMComp->SetCanEverAffectNavigation(false);
}

void ARogueExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	BoxShapeComp->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
}

void ARogueExplosiveBarrel::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor->IsA<ARogueProjectileMagic>())
	{		
		return;
	}	
	
	MulticastStartExplodeSequence();
}

void ARogueExplosiveBarrel::MulticastStartExplodeSequence_Implementation()
{
	Fuse();
}

void ARogueExplosiveBarrel::Fuse()
{
	if(!SpawnedFuseVFX)
	{
		SpawnedFuseVFX = UNiagaraFunctionLibrary::SpawnSystemAttached(FuseVFX, RootComponent, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::SnapToTarget, false);
	}
	else
	{
		SpawnedFuseVFX->ResetSystem();
		SpawnedFuseVFX->Activate(true);
	}
	
	if(!SpawnedFuseSFX)
	{
		SpawnedFuseSFX = UGameplayStatics::SpawnSoundAttached(FuseSFX, RootComponent);
	}
	else
	{
		SpawnedFuseSFX->Play();
	}
	
	FTimerHandle DelayTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &ThisClass::Explode, ExplodeDelay);
}

void ARogueExplosiveBarrel::Explode()
{
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSFX, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplodeVFX, GetActorLocation());

	UGameplayStatics::ApplyRadialDamage(this, 10.f, GetActorLocation(), ExplodeRadialForceComp->Radius, DmgTypeClass, {this});
	
	ExplodeRadialForceComp->FireImpulse();
	
	BoxShapeComp->AddImpulse(FVector::UpVector * 1000.f, NAME_None, true);
	BoxShapeComp->AddAngularImpulseInDegrees(FVector::RightVector * 100.f, NAME_None, true);
	
	FTimerHandle TimerHandle_TurnOffBurnFX;
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_TurnOffBurnFX, 
		FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			if(SpawnedFuseVFX)
			{
				SpawnedFuseVFX->Deactivate();
			}
			
			if(SpawnedFuseSFX)
			{
				SpawnedFuseSFX->Stop();
			}
		}), 1.f, false);
}
