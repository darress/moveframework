#pragma once

#include "MoveConfig.h"
#include "MoveButton.h"

namespace Move
{
	struct MOVE_EXPORT NavData
	{
		int buttons;
		int trigger1;
		int trigger2;
		int stickX;
		int stickY;

		NavData()
		{
			buttons=0;
			trigger1=0;
			trigger2=0;
			stickX=0;
			stickY=0;
		}

		bool isButtonPressed(MoveButton button)
		{
			return (buttons & button)!=0;
		}
	};
}