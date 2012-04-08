#pragma once

#include "EyeImage.h"
#include "MoveIncludes.h"

namespace Move
{
	

	class BallColorManager
	{
		EyeImage* image;
		volatile bool automaticColors;

	public:
		BallColorManager(EyeImage* image);
		~BallColorManager();

		void calculateColors(std::vector<MoveBall>& balls);
		void useAutomaticColors(bool use);
		void setColor(MoveBall& ball, int r, int g, int b);
	};

}