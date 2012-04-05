#pragma once

#include "MoveIncludes.h"

namespace Move
{

	struct ColorRgb
	{
		int r,g,b;
		ColorRgb()
			:r(0),g(0),b(0) {}
		ColorRgb(int r, int g, int b)
			:r(r),g(g),b(b) {}
	};

	struct ColorHsv
	{
		float h,s,v;
		ColorHsv()
			:h(0),s(0),v(0) {}
		ColorHsv(float h, float s, float v)
			:h(h),s(s),v(v) {}

		float similarity(ColorHsv other)
		{
			if (other.v<0.7f)
				return 0.0f;
			float sim;
			sim = abs(h-other.h);
			if (sim>360.0f-sim)
				sim=360.0f-sim;
			if (sim>100)
				return 0.0f;
			sim=100-sim;
			sim=sim*other.s;
			return sim;
		}

		ColorHsv(ColorRgb rgb)
		{
			float r=((float)rgb.r)/255;
			float g=((float)rgb.g)/255;
			float b=((float)rgb.b)/255;

			// RGB are from 0..1, H is from 0..360, SV from 0..1
			float maxC = b;
			if (maxC < g) maxC = g;
			if (maxC < r) maxC = r;
			float minC = b;
			if (minC > g) minC = g;
			if (minC > r) minC = r;

			float delta = maxC - minC;

			v = maxC;
			s = 0;
			h = 0;

			if (delta == 0)
			{
				h = 0;
				s = 0;
			}
			else
			{
				s = delta / maxC;
				float dR = 60*(maxC - r)/delta + 180;
				float dG = 60*(maxC - g)/delta + 180;
				float dB = 60*(maxC - b)/delta + 180;
				if (r == maxC)
					h = dB - dG;
				else if (g == maxC)
					h = 120 + dR - dB;
				else
					h = 240 + dG - dR;
			}

			if (h<0)
				h+=360;
			if (h>=360)
				h-=360;
		}
	};

}