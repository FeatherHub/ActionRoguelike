#include "RogueGameplayTag.h"
#include "NativeGameplayTags.h"

namespace RogueGameplayTag
{
	UE_DEFINE_GAMEPLAY_TAG(Action_Projectile_Magic, TEXT("Action.Projectile.Magic"));	
	UE_DEFINE_GAMEPLAY_TAG(Action_Projectile_Blackhole, TEXT("Action.Projectile.Blackhole"));	
	UE_DEFINE_GAMEPLAY_TAG(Action_Projectile_Teleport, TEXT("Action.Projectile.Teleport"));	
	UE_DEFINE_GAMEPLAY_TAG(Action_Sprint, TEXT("Action.Sprint"));	
	
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Health, TEXT("Attribute.Health"));	
	UE_DEFINE_GAMEPLAY_TAG(Attribute_HealthMax, TEXT("Attribute.HealthMax"));
	UE_DEFINE_GAMEPLAY_TAG(Attribute_RageAmount, TEXT("Attribute.RageAmount"));
	
	UE_DEFINE_GAMEPLAY_TAG(Status_Sprinting, TEXT("Status.Sprinting"));
	UE_DEFINE_GAMEPLAY_TAG(Status_Stunned, TEXT("Status.Stunned"));
	
	UE_DEFINE_GAMEPLAY_TAG(Toast_Channel_Feedback, TEXT("Toast.Channel.Feedback"));
	UE_DEFINE_GAMEPLAY_TAG(Toast_Channel_Notice,   TEXT("Toast.Channel.Notice"));
	UE_DEFINE_GAMEPLAY_TAG(Toast_Channel_Headline, TEXT("Toast.Channel.Headline"));

	UE_DEFINE_GAMEPLAY_TAG(Toast_Sound_Reject,   TEXT("Toast.Sound.Reject"));
	UE_DEFINE_GAMEPLAY_TAG(Toast_Sound_Positive, TEXT("Toast.Sound.Positive"));
	UE_DEFINE_GAMEPLAY_TAG(Toast_Sound_Alarm,    TEXT("Toast.Sound.Alarm"));
}
