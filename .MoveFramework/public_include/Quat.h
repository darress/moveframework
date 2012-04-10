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

		Quat operator!() const;

		void Normalize();

		Vec3 GetColumn0() const {return Vec3(2*(v.x*v.x+w*w)-1,2*(v.y*v.x+v.z*w),2*(v.z*v.x-v.y*w));}
		Vec3 GetColumn1() const {return Vec3(2*(v.x*v.y-v.z*w),2*(v.y*v.y+w*w)-1,2*(v.z*v.y+v.x*w));}
		Vec3 GetColumn2() const {return Vec3(2*(v.x*v.z+v.y*w),2*(v.y*v.z-v.x*w),2*(v.z*v.z+w*w)-1);}
	};

	Quat operator*(const Quat &q, const Quat &p);
	float operator|(const Quat &q, const Quat &p);
	Vec3 operator*(const Vec3 &v, const Quat &q);


}