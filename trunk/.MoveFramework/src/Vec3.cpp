#include "Vec3.h"
#include <cmath>

namespace Move
{
	const Vec3 Vec3::ZERO=Vec3(0.0f,0.0f,0.0f);

	Vec3::Vec3():x(0.0f),y(0.0f),z(0.0f)
	{}

	Vec3::Vec3(float x, float y, float z):x(x),y(y),z(z)
	{}

	float Vec3::length()
	{
		return sqrt(length2());
	}

	float Vec3::length2()
	{
		return x*x+y*y+z*z;
	}

	float Vec3::distance(Vec3 other)
	{
		return (*this-other).length();
	}

	float Vec3::distance2(Vec3 other)
	{
		return (*this-other).length2();
	}

	Vec3 operator+(const Vec3 &a, const Vec3 &b)
	{
		return Vec3(a.x+b.x, a.y+b.y, a.z+b.z);
	}

	Vec3 operator-(const Vec3 &a, const Vec3 &b)
	{
		return Vec3(a.x-b.x, a.y-b.y, a.z-b.z);
	}

	Vec3 operator*(const Vec3 &a, const Vec3 &b)
	{
		return Vec3(a.x*b.x, a.y*b.y, a.z*b.z);
	}

	Vec3 operator*(const Vec3 &a, const float &v)
	{
		return Vec3(a.x*v,a.y*v,a.z*v);
	}

	Vec3 operator*(const float &v, const Vec3 &a)
	{
		return a*v;
	}

	Vec3 operator/(const Vec3 &a, const float &v)
	{
		return Vec3(a.x/v, a.y/v, a.z/v);
	}

	Vec3 operator/(const float &v, const Vec3 &a)
	{
		return Vec3(v/a.x, v/a.y, v/a.z);
	}

	Vec3& operator+=(Vec3 &a, const Vec3 &b){a=a+b; return a;}
	Vec3& operator-=(Vec3 &a, const Vec3 &b){a=a-b; return a;}
	Vec3& operator*=(Vec3 &a, const Vec3 &b){a=a*b; return a;}
	Vec3& operator*=(Vec3 &a, const float &v){a=a*v; return a;}
	Vec3& operator/=(Vec3 &a, const float &v){a=a/v; return a;}
}