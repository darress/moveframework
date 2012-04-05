#pragma once

#include "MoveConfig.h"
#include "MoveData.h"
#include "Quat.h"

namespace Move
{
	class MOVE_EXPORT IMoveController
	{
	public:

		virtual MoveData getMoveData()=0;
		virtual void setRumble(int value)=0;
		virtual void useMagnetometers(bool value)=0;
		virtual void setOrientationGain(float gain)=0;
	};
}