#pragma once

#include "NativeGameplayTags.h"

namespace RogueGameplayTag
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Action_Projectile_Magic);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Action_Projectile_Blackhole);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Action_Projectile_Teleport);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Action_Sprint);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Health);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_HealthMax);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_RageAmount);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Sprinting);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Stunned);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Toast_Channel_Feedback);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Toast_Channel_Notice);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Toast_Channel_Headline);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Toast_Sound_Reject);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Toast_Sound_Positive);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Toast_Sound_Alarm);
}
