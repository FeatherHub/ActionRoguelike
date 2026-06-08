#include "RogueGameMode.h"

#include "ActionRoguelike.h"
#include "EngineUtils.h"
#include "ActionRoguelike/Player/RoguePlayerController.h"
#include "AI/RogueAICharacter.h"
#include "Development/RogueNetUtil.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Misc/DataValidation.h"

static TAutoConsoleVariable<bool> CVarRogueGameModeShowDebug{
	TEXT("rogue.gamemode.ShowDebug"), true,
	TEXT("Show debug info for RogueGameMode"), ECVF_Cheat
};

ARogueGameMode::ARogueGameMode()
{
	PlayerControllerClass = ARoguePlayerController::StaticClass();

	SpawnBotInterval = 3.f;
}

void ARogueGameMode::StartPlay()
{
	Super::StartPlay();
	
	ROGUE_DEBUG_CVAR(CVarRogueGameModeShowDebug, 0, 3.f, FColor::Green, 
		TEXT("[GameMode] StartPlay"))

	GetWorldTimerManager().SetTimer(SpawnBotTimer, this, &ThisClass::SpawnBot, SpawnBotInterval, true);
}

void ARogueGameMode::SpawnBot()
{
	if(!ensure(SpawnBotMaxCurve))
	{
		return;
	}
	
	int32 NumOfAliveBot = 0;
	for(ARogueAICharacter* AICharacter: TActorRange<ARogueAICharacter>(GetWorld()))
	{
		if(AICharacter->IsAlive())
		{
			++NumOfAliveBot;
		}
	}
	
	
	int32 MaxBotCount = FMath::RoundToInt32(SpawnBotMaxCurve->GetFloatValue(GetWorld()->TimeSeconds));
	
	ROGUE_DEBUG_CVARFMT(CVarRogueGameModeShowDebug, 0, 3.f, FColor::Yellow,
		TEXT("[GameMode] Alive Bots: %d / %d at %f"), 
		NumOfAliveBot, MaxBotCount, GetWorld()->TimeSeconds);

	if(NumOfAliveBot >= MaxBotCount)
	{
		return;
	}
	
	if(!IsValid(EnvQueryInstance))
	{
		EnvQueryInstance = UEnvQueryManager::RunEQSQuery(this, EnvQuery_FindBotSpawnLocation, this, 
			EEnvQueryRunMode::RandomBest5Pct, UEnvQueryInstanceBlueprintWrapper::StaticClass());
		
		if(EnvQueryInstance)
		{
			EnvQueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ThisClass::OnEnvQueryFinished);
		}
		else
		{
			ROGUE_DEBUG_CVAR(CVarRogueGameModeShowDebug, 0, 3.f, FColor::Red,
			    TEXT("[GameMode] Fail to run EQSQuery"))
		}
	} 
	else
	{
		ROGUE_DEBUG_CVAR(CVarRogueGameModeShowDebug, 0, 3.f, FColor::Red,
		    TEXT("[GameMode] EnvQueryInstance is already running"))
	}
}

void ARogueGameMode::OnEnvQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	EnvQueryInstance = nullptr;
	
	if(QueryStatus != EEnvQueryStatus::Success)
	{
		ROGUE_DEBUG_CVARFMT(CVarRogueGameModeShowDebug, 0, 3.f, FColor::Red,
		    TEXT("[GameMode] EQS query failed for bot spawn (Status=%s)"), *UEnum::GetValueAsString(QueryStatus));
		return;
	}
	
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if (Locations.IsValidIndex(0))
	{
		FVector SpawnLocation = Locations[0];
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		ARogueAICharacter* SpawnedBot = GetWorld()->SpawnActor<ARogueAICharacter>(BotClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
		if(SpawnedBot)
		{
			ROGUE_DEBUG_CVARFMT(CVarRogueGameModeShowDebug, 0, 3.f, FColor::Blue,
				TEXT("[GameMode] Bot Spawned at %s"), *SpawnLocation.ToString())
		}
	}
}

#if !UE_BUILD_SHIPPING
EDataValidationResult ARogueGameMode::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	
	if(SpawnBotMaxCurve == nullptr)
	{
		Context.AddWarning(FText::FromString(TEXT("SpawnBotMaxCurve is not set")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif
