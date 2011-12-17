#pragma once

#include "MovePrecompiled.h"

namespace Move
{
	struct MOVE_EXPORT MoveData
	{
		Quaternion orientation;
		Vector3 angularVelocity;
		Vector3 angularAcceleration;

		Vector3 position;
		Vector3 velocity;
		Vector3 acceleration;

		bool isOnDisplay;
		Vector3 displayPos;

		int buttons;

		MoveData()
		{
			orientation = Quaternion(1,0,0,0);

			position = Vector3(0,0,0);
			velocity = Vector3(0,0,0);
			acceleration = Vector3(0,0,0);

			isOnDisplay = false;
			displayPos = Vector3(0,0,0);

			buttons=0;
		}
	};
}