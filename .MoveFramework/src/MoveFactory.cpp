#include "MoveFactory.h"
#include "MoveManager.h"

namespace Move
{
	IMoveManager* createDevice()
	{
		return new MoveManager;
	}
}