#pragma once

#include "CoreMinimal.h"
#include "DebugContext.h"
#include "Subsystems/WorldSubsystem.h"
#include "RogueDebugSubsystem.generated.h"

UCLASS()
class ACTIONROGUELIKE_API URogueDebugSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
	
	void Submit(const FOnScreenDebugContext& Context);
	
protected:
	void FlushDebugContextQueue(UWorld* World, ELevelTick LevelTick, float Delta);
	
	/**
      - GEngine->AddOnScreenDebugMessage 전용 UE::GetPlayInEditorID 해싱 메서드
	  - AddOnScreenDebugMessage에 -1을 사용하면 새로운 메시지로 처리되어, 메시지가 마지막 줄에 추가 됨
	  - UE::GetPlayInEditorID 는 PIE 플레이가 아닐 때 음수를 반환함  
	  - 이 메서드는 GetPlayInEditorID에서 반환받은 PIE 번호를 -1이 아닌 값으로 해시함
	*/
	static uint64 HashPlayInEditorID(int32 PlayInEditorID);
	
protected:
	TArray<FOnScreenDebugContext> DebugContextQueue;
	FDelegateHandle WorldTickEndDelegateHandle;
};
