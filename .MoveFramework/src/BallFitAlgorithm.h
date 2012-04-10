#pragma once

#include "MoveIncludes.h"
#include "Vec2.h"
#include "EyeImage.h"
#include "MoveBall.h"

namespace Move
{

	class BallFitAlgorithm
	{
	private:
		EyeImage* img;

	public:
		BallFitAlgorithm(EyeImage* img);
		~BallFitAlgorithm();

		void fitCircle(MoveBall* ball);

	private:
		std::vector<Vec2> currentBallContour;
		float currentBallSize;
		double integrateCircleError(std::vector<double> x);
	};

}