#pragma once

#include "CoreMinimal.h"

void SubmitDebugContext(UObject* ContextObject, uint64 DebugKey, const FString& Msg, const FColor& Color, float Duration);

#if UE_BUILD_SHIPPING || UE_BUILD_TEST
	#define DEBUG_ONSCREEN(DebugSubkey, Duration, Color, Msg)
	#define DEBUG_ONSCREEN_FMT(DebugSubkey, Duration, Color, Fmt, ...)
	#define DEBUG_ONSCREEN_CVAR(CVar, DebugSubkey, Duration, Color, Msg)
	#define DEBUG_ONSCREEN_CVARFMT(CVar, DebugSubkey, Duration, Color, Fmt, ...)
#else
	#define DEBUG_ONSCREEN_CVARFMT(CVar, DebugSubkey, Duration, Color, Fmt, ...) \
		do if(CVar.GetValueOnGameThread()) { \
			DEBUG_ONSCREEN(DebugSubkey, Duration, Color, FString::Printf(Fmt, ##__VA_ARGS__)); \
		} while(false)

	#define DEBUG_ONSCREEN_CVAR(CVar, DebugSubkey, Duration, Color, Msg) \
		do if(CVar.GetValueOnGameThread()) { \
			DEBUG_ONSCREEN(DebugSubkey, Duration, Color, Msg); \
		} while(false)

	#define DEBUG_ONSCREEN_FMT(DebugSubkey, Duration, Color, Fmt, ...) \
		do { \
			DEBUG_ONSCREEN(DebugSubkey, Duration, Color, FString::Printf(Fmt, ##__VA_ARGS__)); \
		} while(false)

	#define DEBUG_ONSCREEN(DebugSubkey, Duration, Color, Msg) \
		do { \
			uint64 Hash1 = HashCombine(GetTypeHash(this), GetTypeHash(DebugSubkey)); \
 			uint64 Hash2 = HashCombine(PointerHash(__FILE__, __LINE__), Hash1); \
			SubmitDebugContext(this, Hash2, Msg, Color, Duration); \
		} while(false)
#endif



// 주의: 중괄호 없는 if 안에 쓰면 안 된다 (그 if 의 else 가 이 DEBUG_IF에 결합된다)
// 주의: else 절을 붙이면 안 된다 (그러면 shipping에서 항상 그 else 블록이 실행된다)
// 주의: 조건문 내에 변수를 선언하면 안 된다 (shipping에서 선언이 사라져 컴파일 에러가 발생한다)
#if UE_BUILD_SHIPPING || UE_BUILD_TEST
	#define DEBUG_IF(Cond) if constexpr(false)
	#define DEBUG_IF_CVAR(CVar) if constexpr(false)
#else 
	#define DEBUG_IF(Cond) if(Cond)
	#define DEBUG_IF_CVAR(CVar) if((CVar).GetValueOnGameThread())
#endif
