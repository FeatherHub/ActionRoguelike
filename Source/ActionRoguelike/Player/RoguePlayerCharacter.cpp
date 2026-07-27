#include "RoguePlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "ActionSystem/RogueActionSystemComponent.h"
#include "ActionSystem/RogueAttributeSet.h"
#include "Core/RogueGameplayTag.h"
#include "DebugSystem/DebugUtil.h"
#include "Net/UnrealNetwork.h"
#include "Network/NetUtil.h"


// For Debugging 
void ARoguePlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	float Health = ActionSystemComp->GetAttributeValue(RogueGameplayTag::Attribute_Health);
	
	DEBUG_ONSCREEN_FMT(0, 0.f, FColor::White, 
		TEXT("[PlayerCharacter::Tick] %s Health: %f"),
		*NetUtil::GetNetName(this), Health);
}

// For Debugging 
void ARoguePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DEBUG_ONSCREEN_FMT(0, 5.f, FColor::White,
		TEXT("[PlayerCharacter::Begin] %s"), *NetUtil::GetNetName(this));
}

// For Debugging 
void ARoguePlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	DEBUG_ONSCREEN_FMT(0, 5.f, FColor::White, 
		TEXT("[PlayerCharacter::PossessedBy] %s"), *NetUtil::GetNetName(this));
}

ARoguePlayerCharacter::ARoguePlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	ActionSystemComp = CreateDefaultSubobject<URogueActionSystemComponent>(TEXT("ActionSystemComp"));
	ActionSystemComp->SetDefaultAttributeSet(URoguePlayerAttributeSet::StaticClass());
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	
	JumpMaxCount = 2;
	
	DamagePerRageRatio = 5.f;
	
	PickupRadius = 128.f;
}


void ARoguePlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ActionSystemComp->GetOnAttributeChangedListener(RogueGameplayTag::Attribute_Health).AddUObject(this, &ThisClass::OnHealthChanged);
	
	GetMesh()->SetOverlayMaterialMaxDrawDistance(1.f);
}

void ARoguePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ThisClass::Look);
	EIC->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &ThisClass::Jump);
	EIC->BindAction(IA_Sprint, ETriggerEvent::Started, this, &ThisClass::StartAction, RogueGameplayTag::Action_Sprint.GetTag());
	EIC->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &ThisClass::StopAction, RogueGameplayTag::Action_Sprint.GetTag());
	
	EIC->BindAction(IA_PrimaryAttack, ETriggerEvent::Triggered, this, &ThisClass::StartAction, RogueGameplayTag::Action_Projectile_Magic.GetTag());
	EIC->BindAction(IA_BlackholeAttack, ETriggerEvent::Triggered, this, &ThisClass::StartAction, RogueGameplayTag::Action_Projectile_Blackhole.GetTag());
	EIC->BindAction(IA_Teleport, ETriggerEvent::Triggered, this, &ThisClass::StartAction, RogueGameplayTag::Action_Projectile_Teleport.GetTag());
}

void ARoguePlayerCharacter::Move(const FInputActionValue& InValue)
{
	FVector2D Value = InValue.Get<FVector2D>();

	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.f;
	
	// forward & backward 
	AddMovementInput(ControlRot.Vector(), Value.X);

	// right & left
	FVector RightDirection = ControlRot.RotateVector(FVector::RightVector);
	AddMovementInput(RightDirection, Value.Y);
}

void ARoguePlayerCharacter::Look(const FInputActionInstance& InInstance)
{
	FVector2D Value = InInstance.GetValue().Get<FVector2D>();

	AddControllerYawInput(Value.X);
	AddControllerPitchInput(Value.Y);
}

void ARoguePlayerCharacter::StartAction(FGameplayTag ActionTag)
{
	ActionSystemComp->StartAction(ActionTag);
}

void ARoguePlayerCharacter::StopAction(FGameplayTag ActionTag)
{
	ActionSystemComp->StopAction(ActionTag);
}

void ARoguePlayerCharacter::OnHealthChanged(float NewHealth, float OldHealth)
{
	if (NewHealth <= 0.f)
	{
		DisableInput(nullptr);

		PlayAnimMontage(AnimMontage_Death);
	}
}

float ARoguePlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	// float ActualDamage =  Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ActionSystemComp->ApplyAttributeChange(RogueGameplayTag::Attribute_Health, -DamageAmount, BaseDelta);
	
	float RageAmount = DamageAmount * DamagePerRageRatio;
	ActionSystemComp->ApplyAttributeChange(RogueGameplayTag::Attribute_RageAmount, RageAmount, BaseDelta);
	
	// Hit Flash
	GetMesh()->SetOverlayMaterialMaxDrawDistance(0.f);
	GetMesh()->SetCustomPrimitiveDataFloat(0, GetWorld()->TimeSeconds);
	GetWorldTimerManager().SetTimer(TimerHandle_HitFlashOverlay, [this](){
		if (IsValid(this))
		{
			GetMesh()->SetOverlayMaterialMaxDrawDistance(1.f);
		}
	}, 1.f, false);
	
	return DamageAmount;
}

void ARoguePlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ARoguePlayerCharacter, ActionSystemComp);
}
