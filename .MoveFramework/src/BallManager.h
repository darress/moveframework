#pragma once

#include "MoveBall.h"
#include "EyeImage.h"
#include "BallColorManager.h"
#include "Vec3.h"
#include "kalman.h"
#include "ContourFinder.h"
#include "BallFitAlgorithm.h" 

namespace Move
{

	class BallManager
	{
		int numMoves;
		std::vector<MoveBall> balls;
		
		EyeImage* img;
		BallColorManager* colorManager;
		ContourFinder* contourFinder;
		BallFitAlgorithm* ballFitAlgorithm;

		Vec3 offset;

		std::vector<Kalman> filter;

	public:
		/**
		 * If this variable is set to a moveId, the offset will be set to the position of ball with that moveId.
		 * If it is -1, it will be ignored.
		 */
		volatile int resetPosition;

	public:
		BallManager(int numMoves, EyeImage* img);
		~BallManager();

		std::vector<Vec3> findBalls();
		unsigned char* getMaskBuffer(int moveId);

		void useAutomaticColors(bool use);
		void setColor(int moveId, int r, int g, int b);

	private:
		Vec3 calculateRealWorldPosition(MoveBall& ball, Kalman& filter);
	};
}