#include "ContourFinder.h"
#include "MoveConfig.h"
#include "MoveExceptions.h"

namespace Move
{

	ContourFinder::ContourFinder(EyeImage* img, int combInterval):img(img)
	{
		myCombInterval = std::chrono::duration<long, std::ratio<1, 1000>>(combInterval >= 0 ? combInterval : COMB_INTERVAL_DEFAULT);
		lastComb = steady_clock::now() - myCombInterval;
	}


	ContourFinder::~ContourFinder()
	{
	}

	void ContourFinder::setCombInterval(int combInterval) {
		if (combInterval >= 0)
			myCombInterval = std::chrono::duration<long, std::ratio<1, 1000>>(combInterval);
	}

	void ContourFinder::findBalls(std::vector<MoveBall*>& balls, int numBalls)
	{
		bool needToComb = false;
		std::chrono::milliseconds mSec;

		for (int i=0; i<numBalls; i++)
		{
			// if the center of the previous ball is in the new ball, don't search for it
			if (balls[i]->ballFound && balls[i]->getMask(balls[i]->position)>BALL_MARGIN)
			{
				balls[i]->position.x--;
				// search the left contour
				searchEdge(balls[i]);
			}
			// if not, we need to comb the whole image
			else
			{
				balls[i]->ballFound=false;
				needToComb=true;
			}
		}
		// comb image if needed
		if (needToComb)
		{
			/*2015-7-13:
			If the previous ball isn't overlapping with the current ball, there is a good chance
			that the ball is outside the camera's view. In this case allowing the camera to comb 
			through the whole image area continuously would result in very high CPU usage. 
			Slowing a bit mitigates this issue. When the duration is over 100 milliseconds the tracking
			starts to lag, without much gain in efficiency.
			*/
			mSec = duration_cast<milliseconds>(steady_clock::now() - lastComb);
			if (mSec > myCombInterval) {
				combImage(balls, numBalls);
				lastComb = steady_clock::now();
			}
		}

		for (int i=0; i<numBalls; i++)
		{
			if (balls[i]->ballFound)
			{
				// find the contour of each ball
				findContour((balls[i]));
				// if the contour is too small, discard it
				if (balls[i]->ballContour.size()<3)
				{
					balls[i]->ballFound=false;
					continue;
				}
			}
		}
	}

	void ContourFinder::findContour(MoveBall* ball)
	{
		int SEARCH_BOX_SIZE;
		SEARCH_BOX_SIZE=(int)(ball->ballSize/15.0f);
		if (SEARCH_BOX_SIZE<1)
			SEARCH_BOX_SIZE=1;

		Vec2 init=ball->position;
		ball->ballContour.clear();

		Vec2 current=init;

		PixelBox box(SEARCH_BOX_SIZE);

		Vec2 min, max;
		min=max=current;
		int currentDir=0;

		for (int i=0;i<10000;i++)
		{
			ball->ballContour.push_back(current);
			if (ball->ballContour.size()>500)
				break;
			
			if (current.x<min.x) min.x=current.x;
			if (current.y<min.y) min.y=current.y;
			if (current.x>max.x) max.x=current.x;
			if (current.y>max.y) max.y=current.y;
			Vec2 dis = current-init;
			if ((abs(dis.x)<float(SEARCH_BOX_SIZE) && abs(dis.y)<float(SEARCH_BOX_SIZE)) && ball->ballContour.size()>20)
				break;
			// find the next point
			Vec2 next = current+box.getPixel(currentDir);
			// if the point is out of the ball
			if (ball->getMask(next)<=BALL_MARGIN)
			{
				bool found=false;
				for (int j=currentDir;j<currentDir+SEARCH_BOX_SIZE*6;j++)
				{
					next = current+box.getPixel(j);
					if(ball->getMask(next)>BALL_MARGIN)
					{
						currentDir=j-1;
						found=true;
						break;
					}
				}
				if (!found)
				{
					ball->ballFound=false;
					return;
				}
			}
			// if the point is in the ball
			else if (ball->getMask(next)>BALL_MARGIN)
			{
				bool found=false;
				for (int j=currentDir;j>currentDir-SEARCH_BOX_SIZE*6;j--)
				{
					next = current+box.getPixel(j);
					if(ball->getMask(next)<=BALL_MARGIN)
					{
						currentDir=j;
						found=true;
						break;
					}
				}
				if (!found)
				{
					ball->ballFound=false;
					return;
				}
			}
			// accurate sub-pixel position
			current.x=floor(current.x);
			current.y=floor(current.y);

			Vec2 sub1=current+box.getPixel(currentDir);
			Vec2 sub2=current+box.getPixel(currentDir+1);
			float mask1=ball->getMask(sub1);
			float mask2=ball->getMask(sub2);
			current=sub1+(sub2-sub1)*(BALL_MARGIN-mask1)/(mask2-mask1);
		}
		ball->position.x=0.5f*(float)(max.x+min.x);
		ball->position.y=0.5f*(float)(max.y+min.y);
		if (max.x-min.x>max.y-min.y)
			ball->ballSize=(float)(max.x-min.x);
		else
			ball->ballSize=(float)(max.y-min.y);
	}

	void ContourFinder::combImage(std::vector<MoveBall*>& balls, int numBalls)
	{
		float searchGap=(float)img->h;
		float searchY;
		while (searchGap>4)
		{
			searchY=searchGap/2;
			while ((int)searchY<img->h)
			{
				// if searchRow finds all the balls, return
				if (searchRow((int)searchY, balls, numBalls))
					return;
				searchY+=searchGap;
			}
			searchGap/=2;
		}
	}

	bool ContourFinder::searchRow(int y, std::vector<MoveBall*>& balls, int numBalls)
	{
		//search in row
		for (int x=0;x<img->w;++x)
		{
			Vec2 currentPos=Vec2(x,y);
			ColorRgb pCol=img->getPixel(currentPos);
			bool allFound=true;
			for (int i=0; i<numBalls; i++)
			{
				ColorHsv ballOut = ColorHsv(balls[i]->ballOutColor);
				if (balls[i]->ballFound)
					continue;
				float sim=ballOut.similarity(ColorHsv(pCol));
				if (sim>0)
				{
					balls[i]->ballPerceptedColor=ColorHsv(pCol);
					balls[i]->position=currentPos;
					// check if there is much similarity in the first 10 pixels
					for (int j=0; j<10; j++)
					{
						currentPos.x+=1;
						if (currentPos.x>=img->w)
							break;
						pCol=img->getPixel(currentPos);
						sim+=ballOut.similarity(ColorHsv(pCol));
					}
					if (sim>400)
					{
						balls[i]->ballFound=true;
						searchEdge(balls[i]);
						continue;
					}
				}
				allFound=false;
			}
			if(allFound)
				return true;
		}
		return false;
	}

	void ContourFinder::searchEdge(MoveBall* ball)
	{
		// search for the edge
		while (ball->getMask(ball->position)>BALL_MARGIN)
		{
			ball->position.x--;
		}
		// accurate sub-pixel position
		float mask1=ball->getMask(ball->position);
		float mask2=ball->getMask(Vec2(ball->position.x+1,ball->position.y));
		ball->position.x=floor(ball->position.x);
		ball->position.y=floor(ball->position.y);
		ball->position.x+=(BALL_MARGIN-mask1)/(mask2-mask1);
	}
}