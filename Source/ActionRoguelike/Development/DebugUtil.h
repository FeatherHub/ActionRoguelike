#pragma once

#define DEBUG_KEY() ((uint64)PointerHash( __FILE__, (__LINE__ * __LINE__)))
#define DEBUG_KEY_NET(IsServer) ((uint64)PointerHash( __FILE__, (__LINE__ * __LINE__ + IsServer)))