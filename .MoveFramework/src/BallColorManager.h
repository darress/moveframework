#pragma once

#include "EyeImage.h"
#include "MoveIncludes.h"

namespace Move
{
	

	class BallColorManager
	{
		EyeImage* image;

	public:
		BallColorManager(EyeImage* image);
		~BallColorManager();

		void calculateColors(std::vector<MoveBall>& balls);
	};

}