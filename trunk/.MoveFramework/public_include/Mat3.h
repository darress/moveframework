#pragma once

#include "MoveConfig.h"
#include "Vec3.h"

namespace Move
{

	struct MOVE_EXPORT Mat3
	{
		float m00, m01, m02, m10, m11, m12, m20, m21, m22;
		Mat3();
		Mat3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22);
	};

	Vec3 operator*(const Vec3 &v, const Mat3 &m);

}