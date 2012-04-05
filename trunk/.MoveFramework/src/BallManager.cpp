#include "BallManager.h"
#include "MoveDevice.h"

namespace Move
{

	BallManager::BallManager(int numMoves, EyeImage* img)
	{
		this->numMoves=numMoves;
		this->img=img;
		offset=Vec3::ZERO;

		balls.resize(numMoves);
		filter.resize(numMoves);

		colorManager = new BallColorManager(img);
	}


	BallManager::~BallManager()
	{
		delete colorManager;
	}

	std::vector<Vec3> BallManager::findBalls()
	{
		std::vector<Vec3> positions;
		colorManager->calculateColors(balls);

		img->findBalls(balls, numMoves);
		for (int i=0; i<numMoves; i++)
		{
			positions.push_back(calculateRealWorldPosition(balls[i], filter[i]));

			MoveDevice::SetMoveColour(i,balls[i].ballOutColor.r,balls[i].ballOutColor.g,balls[i].ballOutColor.b);
		}
		return positions;
	}

	Vec3 BallManager::calculateRealWorldPosition(MoveBall& ball, Kalman& filter)
	{
		
		if (ball.ballFound)
		{
			float x,y,size;
			x=ball.position.x;
			y=ball.position.y;
			size=ball.ballSize;

			img->drawCircle(Vec2((int)x,(int)y),(int)(size/2),ColorRgb(255,0,255));

			Vec3 position;

			position.z=filter.update(2500/size);
			position.x=(x-((float)img->w/2))*position.z/400;
			position.y=(y-((float)img->h/2))*position.z/-400;

			position.x -= offset.x;
			position.y -= offset.y;
			position.z -= offset.z;
			return position;
		}
		else
			return Vec3::ZERO;

	}

}