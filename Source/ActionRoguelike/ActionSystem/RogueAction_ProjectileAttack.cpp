#include "RogueAction_ProjectileAttack.h"

#include "NiagaraFunctionLibrary.h"
#include "Core/RogueDebug.h"
#include "Core/RogueGameType.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile/RogueProjectileBase.h"

URogueAction_ProjectileAttack::URogueAction_ProjectileAttack()
{
	AttackDelay = 0.2f;
}

void URogueAction_ProjectileAttack::StartAction()
{
	Super::StartAction();

	ACharacter* Character = GetOwningCharacter();

	Character->PlayAnimMontage(AnimMontage_Attack);
	
	FVector SpawnLocation = Character->GetMesh()->GetSocketLocation(MuzzleSocketName);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, CastingEffect, SpawnLocation, Character->GetController()->GetControlRotation(), FVector::OneVector, false);
	UGameplayStatics::PlaySound2D(this, CastingSound);
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::SpawnProjectile, AttackDelay, false);
}

void URogueAction_ProjectileAttack::SpawnProjectile()
{
	ACharacter* Character = GetOwningCharacter();
	check(Character)
	
	FVector SpawnLocation = Character->GetMesh()->GetSocketLocation(MuzzleSocketName);
	
	FVector EyeLocation;
	FRotator EyeRotation;
	Character->GetController()->GetPlayerViewPoint(EyeLocation, EyeRotation);
	
	FVector TraceEnd = EyeLocation + EyeRotation.Vector() * 5000.f;
	
	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);

	FVector AdjustedTargetLocation;
	if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, RogueCollision::Trace::Projectile, QueryParams))
	{
		AdjustedTargetLocation = Hit.Location; 
	}
	else
	{
		AdjustedTargetLocation = TraceEnd;
	}
	
	FRotator SpawnRotation = (AdjustedTargetLocation - SpawnLocation).Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = Character;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
	Character->MoveIgnoreActorAdd(SpawnedProjectile);
	
#if !UE_BUILD_SHIPPING
	const float DebugDrawTime = CVarProjectileDrawDebug.GetValueOnGameThread();
	if (DebugDrawTime > 0.0f)
	{
		// line trace 
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Green, false, DebugDrawTime);

		// adjusted projectile path
		DrawDebugLine(GetWorld(), SpawnLocation, AdjustedTargetLocation, FColor::Cyan, false, DebugDrawTime);

		// original projectile path
		DrawDebugLine(GetWorld(), SpawnLocation, SpawnLocation + EyeRotation.Vector() * 5000.f, FColor::Red, false, DebugDrawTime);

		DrawDebugBox(GetWorld(), TraceEnd, FVector{20.f}, FColor::Green, false, DebugDrawTime);
	}
#endif
}
