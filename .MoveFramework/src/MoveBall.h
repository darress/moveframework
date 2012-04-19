#pragma once

#include "Vec2.h"
#include "Vec3.h"
#include "MoveColors.h"
#include "MoveIncludes.h"
#include "EyeImage.h"

namespace Move
{
	class MoveBall
	{
		EyeImage* img;
		unsigned short* mask;

	public:
		MoveBall(EyeImage* img);
		~MoveBall();

		Vec2 position;
		Vec3 previousPosition;
		float ballSize;
		ColorRgb ballOutColor;
		ColorHsv ballPerceptedColor;
		bool ballFound;
		std::list<Vec2> ballContour;	
		
		float getMask(Vec2 pos);
		void resetMask();
		unsigned char* getMaskBuffer();
	};

}