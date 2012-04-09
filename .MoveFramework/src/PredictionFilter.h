#pragma once

#include <vector>
#include "Vec2.h"

namespace Move
{

	class PredictionFilter
	{
		int bufferSize;
		std::vector<Vec2> buffer;
		int position;
		bool initialized;
		float currentTime;

		/// slope of the prediction function
		float m;
		/// intercept of the prediction function
		float b;

	public:
		PredictionFilter();
		PredictionFilter(int bufferSize);
		~PredictionFilter();
		float filter(float value, float dt);

	private:
		float integrateError(std::vector<float> x);
	};

}