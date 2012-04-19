#include "BallManager.h"
#include "MoveDevice.h"
#include "MoveExceptions.h"
#include "MoveLock.h"
#include "IniFile.h"

namespace Move
{

	BallManager::BallManager(int numMoves, EyeImage* img)
	{
		this->numMoves=numMoves;
		this->img=img;
		offset=Vec3::ZERO;
		maskTimer = new short[numMoves];

		resetPosition=-1;

		balls.resize(numMoves);
		for (int i=0; i<numMoves; i++)
		{
			balls[i]=new MoveBall(img);
			maskTimer[i]=0;
		}

		filter.resize(numMoves);

		colorManager = new BallColorManager(img, balls);
		contourFinder = new ContourFinder(img);
		ballFitAlgorithm = new BallFitAlgorithm(img);

		try
		{
			returnZeroIfNotFound=IniFile::GetInt("ReturnZeroIfNotFound", "Tracking", "settings.cfg");

		}
		catch(MoveConfigFileRecordNotFoundException)
		{
			returnZeroIfNotFound=false;
		}
	}


	BallManager::~BallManager()
	{
		delete colorManager;
		delete contourFinder;
		delete ballFitAlgorithm;
		delete[] maskTimer;
	}

	std::vector<Vec3> BallManager::findBalls()
	{
		colorManager->calculateColors();
		
		for (int i=0; i<numMoves; i++)
		{
			balls[i]->resetMask();
			// calculate whole mask if the timer is positive
			if (maskTimer[i]>0)
			{
				for (int ii=0; ii<640; ii++)
				{
					for (int j=0; j<480; j++)
					{
						balls[i]->getMask(Vec2(ii,j));
					}
				}
				maskTimer[i]--;
			}

		}

		contourFinder->findBalls(balls, numMoves);

		std::vector<Vec3> positions;
		for (int i=0; i<numMoves; i++)
		{
			MoveDevice::SetMoveColour(i,balls[i]->ballOutColor.r,balls[i]->ballOutColor.g,balls[i]->ballOutColor.b);

			Vec3 pos;
			if (balls[i]->ballFound)
			{
				ballFitAlgorithm->fitCircle(balls[i]);

				if (balls[i]->ballSize>15 && balls[i]->getMask(balls[i]->position)>BALL_MARGIN)
				{
					balls[i]->ballPerceptedColor=ColorHsv(img->getPixel(balls[i]->position));
				}

				pos=calculateRealWorldPosition(balls[i], filter[i]);

				if (resetPosition==i)
				{
					offset=pos;
					resetPosition=-1;
				}

				pos-=offset;
				//check for NaN
				if (pos.x!=pos.x)
					pos.x=balls[i]->previousPosition.x;
				if (pos.y!=pos.y)
					pos.y=balls[i]->previousPosition.y;
				if (pos.z!=pos.z)
					pos.z=balls[i]->previousPosition.z;
				balls[i]->previousPosition=pos;
			}
			else if (returnZeroIfNotFound)
				pos=Vec3::ZERO;
			else
				pos=balls[i]->previousPosition;

			positions.push_back(pos);
		}
		return positions;
	}

	Vec3 BallManager::calculateRealWorldPosition(MoveBall* ball, Kalman& filter)
	{
		float x,y,size;
		x=ball->position.x;
		y=ball->position.y;
		size=ball->ballSize;

		img->drawCircle(Vec2((int)x,(int)y),(int)(size/2),ColorRgb(255,0,255));

		Vec3 position;

		position.z=filter.update(2500/size);
		//position.z=2500/size;
		position.x=(x-((float)img->w/2))*position.z/400;
		position.y=(y-((float)img->h/2))*position.z/-400;

		return position;
	}

	unsigned char* BallManager::getMaskBuffer(int moveId)
	{
		if (balls.size()<=moveId)
			return 0;
		maskTimer[moveId]=10;
		return balls[moveId]->getMaskBuffer();
	}

	void BallManager::useAutomaticColors(bool use)
	{
		colorManager->useAutomaticColors(use);
	}

	void BallManager::setColor(int moveId, int r, int g, int b)
	{
		MoveLock m(moveId);
		colorManager->setColor(balls[moveId], r, g, b);
	}

}