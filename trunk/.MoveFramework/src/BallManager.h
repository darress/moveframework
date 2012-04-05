#pragma once

#include "MoveBall.h"
#include "EyeImage.h"
#include "BallColorManager.h"
#include "Vec3.h"
#include "kalman.h"

namespace Move
{

	class BallManager
	{
		int numMoves;
		std::vector<MoveBall> balls;
		EyeImage* img;
		BallColorManager* colorManager;
		Vec3 offset;

		std::vector<Kalman> filter;

	public:
		BallManager(int numMoves, EyeImage* img);
		~BallManager();

		std::vector<Vec3> findBalls();

	private:
		Vec3 calculateRealWorldPosition(MoveBall& ball, Kalman& filter);
	};
}