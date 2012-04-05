#pragma once

#include "MoveConfig.h"
#include "Quat.h"
#include "Vec3.h"
#include "MoveButton.h"

namespace Move
{
	struct MOVE_EXPORT MoveData
	{
		Vec3 position;
		Vec3 velocity;
		Vec3 acceleration;

		Quat orientation;
		Vec3 angularVelocity;
		Vec3 angularAcceleration;

		int buttons;
		int trigger;

		MoveData()
		{
			buttons=0;
			trigger=0;
		}

		bool isButtonPressed(MoveButton button)
		{
			return (buttons & button)!=0;
		}
	};
}