#include "PredictionFilter.h"
#include "simplex.h"

namespace Move
{

	PredictionFilter::PredictionFilter()
	{
		bufferSize=0;
	}

	PredictionFilter::PredictionFilter(int bufferSize):bufferSize(bufferSize)
	{
		buffer.resize(bufferSize);
		position=0;
		initialized=false;
		m=0.0f;
		b=0.0f;
		currentTime=0;
	}


	PredictionFilter::~PredictionFilter()
	{
	}

	float PredictionFilter::filter(float value, float dt)
	{
		if (bufferSize==0)
			return value;
		currentTime+=dt;
		buffer[position++]=Vec2(currentTime,value);
		if (position>=bufferSize)
		{
			position=0;
			initialized=true;
		}
		if (!initialized)
			return value;

		//std::vector<float> init;
		//std::vector<float> result;

		//init.push_back(m);
		//init.push_back(b);

		//SpecVectorFunctor<float, PredictionFilter> ft(this, &PredictionFilter::integrateError);
		//result=BT::Simplex(ft, init, 2000);
		//m=result[0];
		//b=result[1];
		
		float sx = 0.0, sy = 0.0, stt = 0.0, sts = 0.0;
		int n = bufferSize;
		for (int i = 0; i < n; ++i)
		{
			sx += buffer[i].x;
			sy += buffer[i].y;
		}
		for (int i = 0; i < n; ++i)
		{
			float t = buffer[i].x - sx/n;
			stt += t*t;
			sts += t*buffer[i].y;
		}

		m = sts/stt;
		b = (sy - sx*m)/n;

		return currentTime*m+b;
	}

	float PredictionFilter::integrateError(std::vector<float> x)
	{
		float error=0.0f;

		for (int i=0; i<bufferSize; i++)
		{
			float diff=buffer[i].x*x[0]+x[1]-buffer[i].y;
			error+=diff*diff;
		}

		return error;
	}
}