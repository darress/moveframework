#pragma once

#include "MoveConfig.h"

namespace Move
{

	struct MOVE_EXPORT Vec2
	{
		float x,y;
		static const Vec2 ZERO;

		Vec2();
		Vec2(float x, float y);
		float length();
		float length2();
		float distance(Vec2 other);
		float distance2(Vec2 other);

		
	};

	Vec2 operator+(const Vec2 &a, const Vec2 &b);
	Vec2 operator-(const Vec2 &a, const Vec2 &b);
	Vec2 operator*(const Vec2 &a, const Vec2 &b);
	Vec2 operator*(const Vec2 &a, const float &v);
	Vec2 operator*(const float &v, const Vec2 &a);
	Vec2 operator/(const Vec2 &a, const float &v);
	Vec2 operator/(const float &v, const Vec2 &a);

	Vec2& operator+=(Vec2 &a, const Vec2 &b);
	Vec2& operator-=(Vec2 &a, const Vec2 &b);
	Vec2& operator*=(Vec2 &a, const Vec2 &b);
	Vec2& operator*=(Vec2 &a, const float &v);
	Vec2& operator/=(Vec2 &a, const float &v);
}