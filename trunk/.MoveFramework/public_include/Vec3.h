#pragma once

#include "MoveConfig.h"

namespace Move
{

	struct MOVE_EXPORT Vec3
	{
		float x,y,z;
		static const Vec3 ZERO;

		Vec3();
		Vec3(float x, float y, float z);
		float length();
		float length2();
		float distance(Vec3 other);
		float distance2(Vec3 other);
	};

	Vec3 operator+(const Vec3 &a, const Vec3 &b);
	Vec3 operator-(const Vec3 &a, const Vec3 &b);
	Vec3 operator*(const Vec3 &a, const Vec3 &b);
	Vec3 operator*(const Vec3 &a, const float &v);
	Vec3 operator*(const float &v, const Vec3 &a);
	Vec3 operator/(const Vec3 &a, const float &v);
	Vec3 operator/(const float &v, const Vec3 &a);

	Vec3& operator+=(Vec3 &a, const Vec3 &b);
	Vec3& operator-=(Vec3 &a, const Vec3 &b);
	Vec3& operator*=(Vec3 &a, const Vec3 &b);
	Vec3& operator*=(Vec3 &a, const float &v);
	Vec3& operator/=(Vec3 &a, const float &v);
}