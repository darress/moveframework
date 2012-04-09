#pragma once

#include "MoveConfig.h"
#include "Vec3.h"

namespace Move
{

	struct MOVE_EXPORT Quat
	{
		Vec3 v;
		float w;

		static const Quat IDENTITY;


		Quat();
		Quat(float w, Vec3 v);
		Quat(float w, float x, float y, float z);
		void Normalize();
	};
}