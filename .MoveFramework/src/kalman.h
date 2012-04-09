#pragma once

namespace Move
{
	class Kalman
	{
		float q; //process noise covariance
		float r; //measurement noise covariance
		float x; //value
		float p; //estimation error covariance
		float k; //kalman gain
		bool firstValue;

	public:
		Kalman()
		{
			p=0.47f;
			q=0.0625f;
			r=4.0f;
			firstValue=true;
		}

		void init(float p, float q, float r)
		{
			this->p=p;
			this->q=q;
			this->r=r;
		}

		float update(float measurement)
		{
			if (measurement!=measurement)
				return x;
			if (firstValue)
			{
				firstValue=false;
				x=measurement;
				return x;
			}
			//prediction update
			//omit x = x
			p = p + q;
 
			//measurement update
			k = p / (p + r);
			x = x + k * (measurement - x);
			p = (1 - k) * p;
			return x;
		}
	};
}