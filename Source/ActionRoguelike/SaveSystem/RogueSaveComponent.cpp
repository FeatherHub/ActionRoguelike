#include "RogueSaveComponent.h"

#include "RogueSaveGame.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

FActorSaveData URogueSaveComponent::GetActorSaveData() const
{
	FActorSaveData ActorSaveData;
	AActor* OwningActor = GetOwner();
	
	ActorSaveData.Id = OwningActor->GetFName();
	ActorSaveData.ActorTransform = OwningActor->GetActorTransform();
	
	FMemoryWriter MemWriter {ActorSaveData.ByteArray};
	FObjectAndNameAsStringProxyArchive Ar {MemWriter, true};
	Ar.ArIsSaveGame = true;
	
	OwningActor->Serialize(Ar);
	
	return ActorSaveData;
}

void URogueSaveComponent::ApplyActorSaveData(const FActorSaveData& ActorSaveData)
{
	AActor* OwningActor = GetOwner();

	OwningActor->SetActorTransform(ActorSaveData.ActorTransform);
	
	FMemoryReader MemReader { ActorSaveData.ByteArray };
	FObjectAndNameAsStringProxyArchive Ar { MemReader, true };
	Ar.ArIsSaveGame = true;
	
	OwningActor->Serialize(Ar);
	
	OnSaveLoaded.Broadcast();
}

FName URogueSaveComponent::GetSaveId() const
{
	return GetOwner()->GetFName();
}
