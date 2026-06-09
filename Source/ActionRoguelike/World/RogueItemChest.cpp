#include "RogueItemChest.h"

#include "Net/UnrealNetwork.h"
#include "SaveSystem/RogueSaveComponent.h"


ARogueItemChest::ARogueItemChest()
{
	bReplicates = true;
	
	BaseMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMeshComp"));
	BaseMeshComp->SetCollisionProfileName(TEXT("Interaction"));
	RootComponent = BaseMeshComp;

	LidMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMeshComp"));
	LidMeshComp->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	LidMeshComp->SetupAttachment(BaseMeshComp);

	SaveComp = CreateDefaultSubobject<URogueSaveComponent>(TEXT("SaveComp"));
}

void ARogueItemChest::BeginPlay()
{
	Super::BeginPlay();
	
	SaveComp->OnSaveLoaded.AddDynamic(this, &ThisClass::OnSaveLoaded);
}

void ARogueItemChest::OnSaveLoaded()
{
	OnIsLidOpenChanged(bIsLidOpen);
}

void ARogueItemChest::Interact_Implementation()
{
	bIsLidOpen = !bIsLidOpen;
	OnIsLidOpenChanged(bIsLidOpen);
}

void ARogueItemChest::OnRep_IsLidOpen()
{
	OnIsLidOpenChanged(bIsLidOpen);
}

void ARogueItemChest::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ARogueItemChest, bIsLidOpen);
}

// void ARogueItemChest::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
// 	
// 	FRotator NewRotator = FMath::RInterpConstantTo(FRotator{AnimationPitchCurrent, 0.f, 0.f}, FRotator{AnimationPitchMax, 0.f, 0.f}, DeltaTime, AnimationPitchSpeed);
// 	AnimationPitchCurrent = NewRotator.Pitch;
//
// 	LidMeshComp->SetRelativeRotation(NewRotator);
// 	
// 	if (FMath::IsNearlyEqual(AnimationPitchCurrent, AnimationPitchMax))
// 	{
// 		SetActorTickEnabled(false);
// 		OnItemChestOpenAnimationComplete();
// 	}
// }