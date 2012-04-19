#pragma once

#include "EyeImage.h"
#include "MoveIncludes.h"
#include "MoveBall.h"
#include "MoveColors.h"

#define NUM_COLORS 6

namespace Move
{
	class BallColorManager
	{
		EyeImage* image;
		volatile bool automaticColors;
		int calibrated;
		static ColorRgb colors[];
		float colorError[NUM_COLORS];
		int currentColor;
		int timer;
		std::vector<MoveBall*>& balls;
		std::set<int> usedColors;
		std::vector<int> ballColors;

		void findBestColor();

	public:
		BallColorManager(EyeImage* image, std::vector<MoveBall*>& balls);
		~BallColorManager();

		void calculateColors();
		void useAutomaticColors(bool use);
		void setColor(MoveBall* ball, int r, int g, int b);
	};

}