#pragma once

#include "MoveConfig.h"
#include "IMoveObserver.h"
#include "Vec3.h"
#include "Quat.h"
#include "IMoveController.h"
#include "IEyeController.h"

namespace Move
{
	class MOVE_EXPORT IMoveManager
	{
	public:

		virtual int initMoves()=0;
		virtual void closeMoves()=0;
		virtual bool initCamera(int numMoves)=0;
		virtual void closeCamera()=0;

		virtual int getNumUsedMoves()=0;
		virtual int getNumAllMoves()=0;

		virtual void subsribe(IMoveObserver* observer)=0;
		virtual void unsubsribe(IMoveObserver* observer)=0;

		virtual int pairMoves()=0;

		virtual IMoveController* getMove(int moveId)=0;
		virtual IEyeController* getEye()=0;
	};
}