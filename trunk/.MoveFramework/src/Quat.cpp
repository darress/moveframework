#include "Quat.h"

namespace Move
{
	const Quat Quat::IDENTITY=Quat(1.0f,Vec3::ZERO);

	Quat::Quat():w(0.0f),v(Vec3::ZERO)
	{}

	Quat::Quat(float w, Vec3 v):w(w),v(v)
	{}

	Quat::Quat(float w, float x, float y, float z):w(w),v(Vec3(x,y,z))
	{}

}