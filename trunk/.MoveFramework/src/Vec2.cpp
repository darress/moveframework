#include "Vec2.h"
#include <cmath>

namespace Move
{
	const Vec2 Vec2::ZERO=Vec2(0.0f,0.0f);

	Vec2::Vec2():x(0.0f),y(0.0f)
	{}

	Vec2::Vec2(float x, float y):x(x),y(y)
	{}

	float Vec2::length()
	{
		return sqrt(length2());
	}

	float Vec2::length2()
	{
		return x*x+y*y;
	}

	float Vec2::distance(Vec2 other)
	{
		return (*this-other).length();
	}

	float Vec2::distance2(Vec2 other)
	{
		return (*this-other).length2();
	}

	Vec2 operator+(const Vec2 &a, const Vec2 &b)
	{
		return Vec2(a.x+b.x, a.y+b.y);
	}

	Vec2 operator-(const Vec2 &a, const Vec2 &b)
	{
		return Vec2(a.x-b.x, a.y-b.y);
	}

	Vec2 operator*(const Vec2 &a, const Vec2 &b)
	{
		return Vec2(a.x*b.x, a.y*b.y);
	}

	Vec2 operator*(const Vec2 &a, const float &v)
	{
		return Vec2(a.x*v,a.y*v);
	}

	Vec2 operator*(const float &v, const Vec2 &a)
	{
		return a*v;
	}

	Vec2 operator/(const Vec2 &a, const float &v)
	{
		return Vec2(a.x/v, a.y/v);
	}

	Vec2 operator/(const float &v, const Vec2 &a)
	{
		return Vec2(v/a.x, v/a.y);
	}

	Vec2& operator+=(Vec2 &a, const Vec2 &b){a=a+b; return a;}
	Vec2& operator-=(Vec2 &a, const Vec2 &b){a=a-b; return a;}
	Vec2& operator*=(Vec2 &a, const Vec2 &b){a=a*b; return a;}
	Vec2& operator*=(Vec2 &a, const float &v){a=a*v; return a;}
	Vec2& operator/=(Vec2 &a, const float &v){a=a/v; return a;}
}