#pragma once

#include "CoreTypes.h"

enum class ENetControlStatus: uint8
{
	NA, // NotAvailable 
	Local, // LocallyControlled 
	NotLocal // RemotelyControlled
};