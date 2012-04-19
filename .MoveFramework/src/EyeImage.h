#pragma once

#include "MoveIncludes.h"
#include "Vec2.h"
#include "MoveColors.h"

namespace Move
{

	struct EyeImage
	{
		PBYTE data;
		int w,h;

		EyeImage(int w, int h);
		~EyeImage(void);

		void setPixel(Vec2 pos, ColorRgb rgb);
		ColorRgb getPixel(Vec2 pos);
		void drawCircle(Vec2 center, int r, ColorRgb col);
	};
}
