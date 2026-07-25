#include "RogueAction_ProjectileAttack.h"

#include "NiagaraFunctionLibrary.h"
#include "Core/RogueGameType.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile/RogueProjectileBase.h"
#include "Projectile/RogueProjectileConsoleVariable.h"

URogueAction_ProjectileAttack::URogueAction_ProjectileAttack()
{
	AttackDelay = 0.2f;
}

void URogueAction_ProjectileAttack::StartAction_Implementation()
{
	Super::StartAction_Implementation();

	ACharacter* Character = GetOwningCharacter();
	
	Character->PlayAnimMontage(AnimMontage_Attack);
	
	FVector SpawnLocation = Character->GetMesh()->GetSocketLocation(MuzzleSocketName);
	FRotator SpawnRotation = Character->GetActorRotation();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, CastingEffect, SpawnLocation, SpawnRotation, FVector::OneVector, false);
	UGameplayStatics::PlaySound2D(this, CastingSound);
	
	if(Character->HasAuthority())
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::SpawnProjectile, AttackDelay, false);
	}
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
	
	StopAction();
	
#if !UE_BUILD_SHIPPING
	const float DebugDrawTime = CVarProjectileDrawDebug.GetValueOnGameThread();
	if (DebugDrawTime > 0.0f)
	{
		// line trace 
		DrawDebugDirectionalArrow(GetWorld(), EyeLocation, TraceEnd, 48, FColor::Green, false, DebugDrawTime);

		// adjusted projectile path
		DrawDebugDirectionalArrow(GetWorld(), SpawnLocation, AdjustedTargetLocation, 48, FColor::Cyan, false, DebugDrawTime);

		// original projectile path
		DrawDebugDirectionalArrow(GetWorld(), SpawnLocation, SpawnLocation + EyeRotation.Vector() * 5000.f, 48, FColor::Red, false, DebugDrawTime);

		DrawDebugBox(GetWorld(), TraceEnd, FVector{20.f}, FColor::Green, false, DebugDrawTime);
	}
#endif
}
