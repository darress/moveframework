#pragma once

#include "MoveConfig.h"
#include "IMoveManager.h"

namespace Move
{
	MOVE_EXPORT IMoveManager* createDevice();
}