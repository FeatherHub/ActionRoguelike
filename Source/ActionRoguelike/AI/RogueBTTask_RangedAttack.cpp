#include "RogueBTTask_RangedAttack.h"

#include "AIController.h"
#include "RogueAIConsoleVariable.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Projectile/RogueProjectileBase.h"

EBTNodeResult::Type URogueBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();

	AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!IsValid(TargetActor))
	{
		return EBTNodeResult::Failed;
	}
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	ACharacter* AICharacter = Cast<ACharacter>(AIController->GetPawn());
	FVector SpawnLocation = AICharacter->GetMesh()->GetSocketLocation(MuzzleSocketName);

	FRotator SpawnRotation = (TargetActor->GetActorLocation() - SpawnLocation).Rotation();
	SpawnRotation.Pitch += FMath::RandRange(0.f, BulletSpreadDegree);
	SpawnRotation.Yaw += FMath::RandRange(-BulletSpreadDegree, BulletSpreadDegree);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = AICharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* NewProjectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (CVarMinionRangedDrawDebug.GetValueOnGameThread())
	{
		DrawDebugDirectionalArrow(GetWorld(), SpawnLocation, TargetActor->GetActorLocation(), 48, FColor::Black, false, 3.0f, 2.f);
	}
	
	return NewProjectile ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
