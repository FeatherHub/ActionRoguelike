#include "RogueToastMessageFactory.h"
#include "Core/RogueGameplayTag.h"

#define LOCTEXT_NAMESPACE "RogueToastMessage"

namespace RogueToastMessageFactory
{
	FRogueToastMessage ActionCooldown(const FText& ActionName, int32 Seconds)
	{
		FRogueToastMessage Message;
		Message.ChannelTag   = RogueGameplayTag::Toast_Channel_Feedback;
		Message.SoundTag  = RogueGameplayTag::Toast_Sound_Reject;
		Message.Body      = FText::Format(LOCTEXT("ActionCooldown", "{0} 쿨타임 {1}초"), ActionName, Seconds);
		return Message;
	}

	FRogueToastMessage ActionBlocked(const FText& ActionName)
	{
		FRogueToastMessage Message;
		Message.ChannelTag   = RogueGameplayTag::Toast_Channel_Feedback;
		Message.SoundTag  = RogueGameplayTag::Toast_Sound_Reject;
		Message.Body      = FText::Format(LOCTEXT("ActionBlocked", "지금은 {0}을(를) 쓸 수 없습니다"), ActionName);
		return Message;
	}

	FRogueToastMessage ActionCostShort(const FText& ActionName, int32 Missing)
	{
		FRogueToastMessage Message;
		Message.ChannelTag   = RogueGameplayTag::Toast_Channel_Feedback;
		Message.SoundTag  = RogueGameplayTag::Toast_Sound_Reject;
		Message.Body      = FText::Format(LOCTEXT("ActionCostShort", "{0}: 자원이 {1} 부족합니다"), ActionName, Missing);
		return Message;
	}

	FRogueToastMessage ActionNotGranted(const FText& ActionName)
	{
		FRogueToastMessage Message;
		Message.ChannelTag   = RogueGameplayTag::Toast_Channel_Feedback;
		Message.SoundTag  = RogueGameplayTag::Toast_Sound_Reject;
		Message.Body      = FText::Format(LOCTEXT("ActionNotGranted", "{0}을(를) 아직 배우지 못했습니다"), ActionName);
		return Message;
	}

	FRogueToastMessage LevelUp(int32 NewLevel)
	{
		FRogueToastMessage Message;
		Message.ChannelTag   = RogueGameplayTag::Toast_Channel_Notice;
		Message.SoundTag  = RogueGameplayTag::Toast_Sound_Positive;
		Message.Body      = FText::Format(LOCTEXT("LevelUp", "레벨 {0} 달성"), NewLevel);
		return Message;
	}

	FRogueToastMessage EnemySurge(int32 Count)
	{
		FRogueToastMessage Message;
		Message.ChannelTag   = RogueGameplayTag::Toast_Channel_Notice;
		Message.SoundTag  = RogueGameplayTag::Toast_Sound_Alarm;
		Message.Body      = FText::Format(LOCTEXT("EnemySurge", "위험: 적이 {0}마리를 넘었습니다"), Count);
		return Message;
	}

	FRogueToastMessage BossAppear(const FText& BossName)
	{
		FRogueToastMessage Message;
		Message.ChannelTag   = RogueGameplayTag::Toast_Channel_Headline;
		Message.SoundTag  = RogueGameplayTag::Toast_Sound_Alarm;
		Message.Body      = FText::Format(LOCTEXT("BossAppear", "{0} 등장"), BossName);
		return Message;
	}

	FRogueToastMessage GameStart()
	{
		FRogueToastMessage Message;
		Message.ChannelTag   = RogueGameplayTag::Toast_Channel_Headline;
		Message.Body      = LOCTEXT("GameStart", "게임 시작");
		return Message;
	}
}

#undef LOCTEXT_NAMESPACE