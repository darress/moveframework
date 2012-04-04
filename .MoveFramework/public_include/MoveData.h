#pragma once

#include "Quat.h"

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



		bool isOnDisplay;
		Vec3 displayPos;

		int buttons;
		int trigger;

		MoveData()
		{
			orientation = Quat(1,0,0,0);

			position = Vec3(0,0,0);
			velocity = Vec3(0,0,0);
			acceleration = Vec3(0,0,0);

			isOnDisplay = false;
			displayPos = Vec3(0,0,0);

			buttons=0;
		}
	};
}