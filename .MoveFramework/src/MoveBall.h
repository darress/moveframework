#pragma once

#include "Vec2.h"
#include "Vec3.h"
#include "MoveColors.h"
#include "MoveIncludes.h"

namespace Move
{

	class MoveBall
	{
	public:
		MoveBall();
		~MoveBall();

		Vec2 position;
		Vec3 previousPosition;
		float ballSize;
		ColorRgb ballOutColor;
		ColorHsv ballPerceptedColor;
		bool ballFound;
		std::list<Vec2> ballContour;
		unsigned short* mask;
	};

}