#include "BallColorManager.h"
#include "MoveColors.h"

namespace Move
{

	BallColorManager::BallColorManager(EyeImage* image):image(image)
	{
		automaticColors=true;
	}


	BallColorManager::~BallColorManager()
	{
	}

	void BallColorManager::calculateColors(std::vector<MoveBall>& balls)
	{
		if (!automaticColors)
			return;
		for (int i=0; i<balls.size(); i++)
		{
			if (i==0)
				balls[i].ballOutColor=ColorRgb(0,0,255);
			else if (i==1)
				balls[i].ballOutColor=ColorRgb(0,255,0);
				
		}
	}

	void BallColorManager::useAutomaticColors(bool use)
	{
		automaticColors=use;
	}

	void BallColorManager::setColor(MoveBall& ball, int r, int g, int b)
	{
		if (!automaticColors)
		{
			ball.ballOutColor=ColorRgb(r,g,b);
		}
	}

}