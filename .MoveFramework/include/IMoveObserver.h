#pragma once

#include "MovePrecompiled.h"
#include "MoveData.h"

namespace Move
{
	class MOVE_EXPORT IMoveObserver
	{
	public:
		virtual void moveUpdated(int moveId, MoveData data){}
		virtual void moveKeyPressed(int moveId, int keyCode){}
		virtual void moveKeyReleased(int moveId, int keyCode){}
	};
}