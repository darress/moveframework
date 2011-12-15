#pragma once

#include "MovePrecompiled.h"

namespace Move
{
	class MOVE_EXPORT IMoveCalibrationObserver
	{
	public:
		virtual void calibrationDone(int moveId)=0;
	};
}