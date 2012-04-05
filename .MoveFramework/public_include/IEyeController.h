#pragma once

#include "MoveConfig.h"

namespace Move
{
	class MOVE_EXPORT IEyeController
	{
	public:

		virtual unsigned char* getEyeBuffer()=0;
		virtual void getEyeDimensions(int &x, int &y)=0;
		virtual void useAutomaticColors(bool use)=0;
		virtual void setColor(int moveId, int r, int g, int b)=0;
		virtual void resetPosition(int moveId)=0;
	};
}