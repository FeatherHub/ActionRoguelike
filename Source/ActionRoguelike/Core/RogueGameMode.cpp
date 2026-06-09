#include "RogueGameMode.h"

#include "ActionRoguelike.h"
#include "EngineUtils.h"
#include "RogueInteractionInterface.h"
#include "RoguePlayerState.h"
#include "RogueSaveGame.h"
#include "ActionRoguelike/Player/RoguePlayerController.h"
#include "AI/RogueAICharacter.h"
#include "Development/RogueNetUtil.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/GameState.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DataValidation.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

static TAutoConsoleVariable<bool> CVarSpawnBotShowDebug{
	TEXT("rogue.gamemode.spawnbot.ShowDebug"), false,
	TEXT("Show spawn bot related debug info"), ECVF_Cheat
};

static TAutoConsoleVariable<bool> CVarSaveSystemShowDebug{
	TEXT("rogue.gamemode.savesystem.ShowDebug"), true,
	TEXT("Show save system related debug info"), ECVF_Cheat
};

//////////////
// Lifecycle
ARogueGameMode::ARogueGameMode()
{
	PlayerControllerClass = ARoguePlayerController::StaticClass();
	PlayerStateClass = ARoguePlayerState::StaticClass();
	
	SpawnBotInterval = 3.f;

	SaveSlotName = TEXT("SaveSlot01");
}

void ARogueGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	LoadSaveGameObject();
}


void ARogueGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	
	if(ARoguePlayerState* PlayerState = NewPlayer->GetPlayerState<ARoguePlayerState>())
	{
		PlayerState->Load(CurrentSaveGame);
	}
}

void ARogueGameMode::StartPlay()
{
	Super::StartPlay();

	LoadSavedActors();
	
	ROGUE_DEBUG_CVAR(CVarSpawnBotShowDebug, 0, 3.f, FColor::Green, 
		TEXT("[GameMode] StartPlay"))

	GetWorldTimerManager().SetTimer(SpawnBotTimer, this, &ThisClass::SpawnBot, SpawnBotInterval, true);
}


///////////////
// Spawn Bot
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
	
	ROGUE_DEBUG_CVARFMT(CVarSpawnBotShowDebug, 0, 3.f, FColor::Yellow,
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
			ROGUE_DEBUG_CVAR(CVarSpawnBotShowDebug, 0, 3.f, FColor::Red,
			    TEXT("[GameMode] Fail to run EQSQuery"))
		}
	} 
	else
	{
		ROGUE_DEBUG_CVAR(CVarSpawnBotShowDebug, 0, 3.f, FColor::Red,
		    TEXT("[GameMode] EnvQueryInstance is already running"))
	}
}

void ARogueGameMode::OnEnvQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	EnvQueryInstance = nullptr;
	
	if(QueryStatus != EEnvQueryStatus::Success)
	{
		ROGUE_DEBUG_CVARFMT(CVarSpawnBotShowDebug, 0, 3.f, FColor::Red,
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
			ROGUE_DEBUG_CVARFMT(CVarSpawnBotShowDebug, 0, 3.f, FColor::Blue,
				TEXT("[GameMode] Bot Spawned at %s"), *SpawnLocation.ToString())
		}
	}
}

////////////////
// Save System
bool ARogueGameMode::WriteToSaveGameObject()
{
	if(!ensure(CurrentSaveGame))
	{
		return false;
	}
	
	auto& AllPlayerState = GameState->PlayerArray;
	for (int i = 0; i < AllPlayerState.Num(); ++i)
	{
		if(ARoguePlayerState* PlayerState = Cast<ARoguePlayerState>(AllPlayerState[i]))
		{
			PlayerState->SaveGame(CurrentSaveGame);
		}
	}


	CurrentSaveGame->ActorSaveDataArray.Empty();
	
	for (AActor* Actor : FActorRange{GetWorld()})
	{
		if(!Actor->Implements<URogueInteractionInterface>())
		{
			continue;
		}
		
		FActorSaveData ActorSaveData;
		ActorSaveData.ActorName = Actor->GetFName();
		ActorSaveData.ActorTransform = Actor->GetActorTransform();

		FMemoryWriter MemWriter {ActorSaveData.ByteArray};
		FObjectAndNameAsStringProxyArchive Ar {MemWriter, true};
		Actor->Serialize(Ar);
		
		CurrentSaveGame->ActorSaveDataArray.Add(ActorSaveData);
	}
	
	bool bSaveSucceed = UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SaveSlotName, 0);
	
	ROGUE_DEBUG_CVARFMT(CVarSaveSystemShowDebug, 0, 3.f, bSaveSucceed ? FColor::Green : FColor::Red, 
	    TEXT("[GameMode] Save Game Result: %d"), bSaveSucceed);
	
	return bSaveSucceed;
}

void ARogueGameMode::LoadSavedActors()
{
	if(!CurrentSaveGame || CurrentSaveGame->ActorSaveDataArray.IsEmpty())
	{
		return;
	}
	
	// Pointers into ActorSaveDataArray to accelerate Actor look up
	// Do not modify the array below this line
	TMap<FName, const FActorSaveData*> ActorSaveDataMap;
	for (const FActorSaveData& ActorSaveData : CurrentSaveGame->ActorSaveDataArray)
	{
		ActorSaveDataMap.Add(ActorSaveData.ActorName, &ActorSaveData);
	}
		
	for (AActor* Actor : FActorRange{GetWorld()})
	{
		if(!Actor->Implements<URogueInteractionInterface>())
		{
			continue;
		}

		if(const FActorSaveData** ActorSaveData = ActorSaveDataMap.Find(Actor->GetFName()))
		{
			Actor->SetActorTransform((*ActorSaveData)->ActorTransform);
			
			FMemoryReader MemReader {(*ActorSaveData)->ByteArray};
			FObjectAndNameAsStringProxyArchive Ar {MemReader, true};
			Actor->Serialize(Ar);
		}
	}
}

void ARogueGameMode::LoadSaveGameObject()
{
	if(UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		CurrentSaveGame = Cast<URogueSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}
	else
	{
		CurrentSaveGame = Cast<URogueSaveGame>(UGameplayStatics::CreateSaveGameObject(URogueSaveGame::StaticClass()));
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
