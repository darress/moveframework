#pragma once

#include "MoveConfig.h"

namespace Move
{
	enum MOVE_EXPORT MoveButton
	{
		B_NONE=		 0x0,
		B_T1=        0x1,
		B_T2=        0x4,
		B_TRIANGLE= 0x10,
		B_CIRCLE=   0x20,
		B_CROSS=    0x40,
		B_SQUARE=   0x80,
		B_SELECT=  0x100,
		B_START=   0x800,
		B_STICK=   0x200,
		B_UP=     0x1000,
		B_RIGHT=  0x2000,
		B_DOWN=   0x4000,
		B_LEFT=   0x8000,
		B_PS=    0x10000,
		B_MOVE=  0x80000,
		B_T=    0x100000
	};
}