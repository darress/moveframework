#pragma once

#include "MoveConfig.h"
#include "NavData.h"

namespace Move
{
	class MOVE_EXPORT INavController
	{
	public:
		virtual NavData getNavData()=0;
	};
}