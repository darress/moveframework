#include "MovePrecompiled.h"
#include "MoveFactory.h"
#include "IniFile.h"

namespace Move
{
	IMoveManager* createDevice()
	{
		return new MoveManager;
	}
}