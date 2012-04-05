#pragma once

#include "MoveIncludes.h"
#include "Vec2.h"
#include "MoveBall.h"
#include "MoveColors.h"

namespace Move
{

	struct EyeImage
	{
		volatile PBYTE data;
		volatile int w,h;

		EyeImage(int w, int h, PBYTE imgBuffer);
		~EyeImage(void);

		void setPixel(Vec2 pos, ColorRgb rgb);
		ColorRgb getPixel(Vec2 pos);
		void drawCircle(Vec2 center, int r, ColorRgb col);
		void findBalls(std::vector<MoveBall>& balls, int numBalls);

	private:
		void combImage(std::vector<MoveBall>& balls, int numBalls);
		bool searchRow(int y, Vec2 &found, std::vector<MoveBall>& balls, int numBalls);
		bool colorMatches(ColorRgb pixelColor, ColorHsv ballColor);
		bool pixelMatches(Vec2 pos, ColorHsv ballColor);
		bool pixelAlreadyFound(Vec2 pos);
		void findContour(MoveBall* ball);
		void fitCircle(MoveBall* ball);

		static std::vector<Vec2> currentBallContour;
		static float currentBallSize;
		static double integrateCircleError(std::vector<double> x);

		Vec2 findNeighbor(Vec2 pos, MoveBall* ball);
		bool checkIfContour(Vec2 pos, MoveBall* ball);
	};
}
