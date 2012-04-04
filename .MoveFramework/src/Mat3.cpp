#include "Mat3.h"

namespace Move
{

	Mat3::Mat3():
		m00( 0.0f ), m01( 0.0f ), m02( 0.0f ), m10( 0.0f ), m11( 0.0f ), m12( 0.0f ), m20( 0.0f ), m21( 0.0f ), m22( 0.0f )
	{}

	Mat3::Mat3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22):
		m00( m00 ), m01( m01 ), m02( m02 ), m10( m10 ), m11( m11 ), m12( m12 ), m20( m20 ), m21( m21 ), m22( m22 )
	{}

	Vec3 operator*(const Vec3 &v, const Mat3 &m)
	{
		return Vec3(v.x*m.m00 + v.y*m.m01 + v.z*m.m02,
					v.x*m.m10 + v.y*m.m11 + v.z*m.m12,
					v.x*m.m20 + v.y*m.m21 + v.z*m.m22);
	}
}