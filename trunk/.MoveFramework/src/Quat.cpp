#include "Quat.h"
#include "MoveIncludes.h"

namespace Move
{
	const Quat Quat::IDENTITY=Quat(1.0f,Vec3::ZERO);

	Quat::Quat():w(1.0f),v(Vec3::ZERO)
	{}

	Quat::Quat(float w, Vec3 v):w(w),v(v)
	{}

	Quat::Quat(float w, float x, float y, float z):w(w),v(Vec3(x,y,z))
	{}

	Quat Quat::operator!() const
	{
		return Quat(w,-1.0f*v);
	}

	void Quat::Normalize()
	{
		float fInvLen = 1.0f/sqrt( v.x*v.x + v.y*v.y + v.z*v.z + w*w );
		v.x*=fInvLen; v.y*=fInvLen; v.z*=fInvLen; w *= fInvLen;
	}

	Quat operator*(const Quat &q, const Quat &p)
	{
		return Quat( 
			q.w*p.w  - (q.v.x*p.v.x + q.v.y*p.v.y + q.v.z*p.v.z),
			q.v.y*p.v.z-q.v.z*p.v.y + q.w*p.v.x + q.v.x*p.w,
			q.v.z*p.v.x-q.v.x*p.v.z + q.w*p.v.y + q.v.y*p.w,
			q.v.x*p.v.y-q.v.y*p.v.x + q.w*p.v.z + q.v.z*p.w  
		);
	}

	float operator|(const Quat &q, const Quat &p)
	{
		return (q.v.x*p.v.x + q.v.y*p.v.y + q.v.z*p.v.z + q.w*p.w); 
	}

	Vec3 operator*(const Vec3 &v, const Quat &q)
	{
		Vec3 out,r2;
		r2.x=(q.v.z*v.y-q.v.y*v.z)+q.w*v.x;
		r2.y=(q.v.x*v.z-q.v.z*v.x)+q.w*v.y;
		r2.z=(q.v.y*v.x-q.v.x*v.y)+q.w*v.z;
		out.x=(r2.y*q.v.z-r2.z*q.v.y); out.x+=out.x+v.x;
		out.y=(r2.z*q.v.x-r2.x*q.v.z); out.y+=out.y+v.y;
		out.z=(r2.x*q.v.y-r2.y*q.v.x); out.z+=out.z+v.z;
		return out;
	}
}