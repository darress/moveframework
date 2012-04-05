#include "BallColorManager.h"

namespace Move
{

	BallColorManager::BallColorManager(EyeImage* image)
	{
		this->image=image;
	}


	BallColorManager::~BallColorManager()
	{
	}

	void BallColorManager::calculateColors(std::vector<MoveBall>& balls)
	{
		for (int i=0; i<balls.size(); i++)
		{
			if (i==0)
				balls[i].ballOutColor=ColorRgb(0,0,255);
			else if (i==1)
				balls[i].ballOutColor=ColorRgb(0,255,0);
				
		}
	}

}