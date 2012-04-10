#include "BallFitAlgorithm.h"
#include "simplex.h"

namespace Move
{
	BallFitAlgorithm::BallFitAlgorithm(EyeImage* img)
		:img(img)
	{
	}


	BallFitAlgorithm::~BallFitAlgorithm()
	{
	}

	void BallFitAlgorithm::fitCircle(MoveBall* ball)
	{
		currentBallContour.clear();
		
		int skipmax=4;
		int skip=skipmax;
		std::list<Vec2>::iterator it;
		for ( it=ball->ballContour.begin() ; it != ball->ballContour.end(); it++ )
		{
			skip--;
			if (skip==0)
				skip=skipmax;
			else
				continue;
			Vec2 p=*it;
			if (p.x>2 && p.y>1 && p.x<img->w-1 && p.y<img->h-1)
			{
				Vec2 pp=Vec2(p.x,p.y);
				currentBallContour.push_back(pp);
			}
		}

		std::vector<double> init;
		std::vector<double> result;

		init.push_back(ball->position.x);
		init.push_back(ball->position.y);
		currentBallSize= (ball->ballSize)*0.5f;

		result.clear();
		SpecVectorFunctor<double,BallFitAlgorithm> ft(this, &BallFitAlgorithm::integrateCircleError);
		result=BT::Simplex(ft, init, 30);

		ball->position.x=(float)result[0];
		ball->position.y=(float)result[1];

		float size=0;
		int nums=0;
		Vec2 midp=Vec2(result[0],result[1]);
		for (unsigned int i=0; i<currentBallContour.size(); i++)
		{
			float dis=midp.distance(currentBallContour[i]);
			if (dis>currentBallSize*0.9f)
			{
				size+=dis;
				nums++;
			}
		}
		ball->ballSize=(size/(float)nums)*2.0f;
	}

	double BallFitAlgorithm::integrateCircleError(std::vector<double> x)
	{
		double error=0.0;

		Vec2 midp=Vec2(x[0],x[1]);

		for (unsigned int i=0; i<currentBallContour.size(); i++)
		{
			double dis=midp.distance2(currentBallContour[i]);
			error+=abs(dis-currentBallSize*currentBallSize);
		}

		return error;
	}
}