#include "ContourFinder.h"
#include "MoveConfig.h"
#include "MoveExceptions.h"

namespace Move
{

	ContourFinder::ContourFinder(EyeImage* img):img(img)
	{
	}


	ContourFinder::~ContourFinder()
	{
	}

	void ContourFinder::findBalls(std::vector<MoveBall>& balls, int numBalls)
	{
		bool needToComb = false;
		for (int i=0; i<numBalls; i++)
		{
			memset(balls[i].mask,0,img->w*img->h*2);

			//ONLY TESTING
			//for (int ii=0; ii<640; ii++)
			//{
			//	for (int j=0; j<480; j++)
			//	{
			//		getMask(Vec2(ii,j),balls[i]);
			//	}
			//}


			// if the center of the previous ball is in the new ball, don't search for it
			if (balls[i].ballFound && getMask(balls[i].position,balls[i])>BALL_MARGIN)
			{
				balls[i].position.x--;
				// search the left contour
				searchEdge(balls[i]);
			}
			// if not, we need to comb the whole image
			else
			{
				balls[i].ballFound=false;
				needToComb=true;
			}
		}
		// comb image if needed
		if (needToComb)
		{
			combImage(balls, numBalls);
		}

		for (int i=0; i<numBalls; i++)
		{
			if (balls[i].ballFound)
			{
				// find the contour of each ball
				findContour((balls[i]));
				// if the contour is too small, discard it
				if (balls[i].ballContour.size()<3)
				{
					balls[i].ballFound=false;
					continue;
				}
			}
		}
	}

	void ContourFinder::findContour(MoveBall& ball)
	{
		int SEARCH_BOX_SIZE;
		SEARCH_BOX_SIZE=(int)(ball.ballSize/15.0f);
		if (SEARCH_BOX_SIZE<1)
			SEARCH_BOX_SIZE=1;

		Vec2 init=ball.position;
		ball.ballContour.clear();

		Vec2 current=init;

		PixelBox box(SEARCH_BOX_SIZE);

		Vec2 min, max;
		min=max=current;
		int currentDir=0;

		for (int i=0;i<10000;i++)
		{
			ball.ballContour.push_back(current);
			if (ball.ballContour.size()>500)
				break;
			
			if (current.x<min.x) min.x=current.x;
			if (current.y<min.y) min.y=current.y;
			if (current.x>max.x) max.x=current.x;
			if (current.y>max.y) max.y=current.y;
			Vec2 dis = current-init;
			if ((abs(dis.x)<float(SEARCH_BOX_SIZE) && abs(dis.y)<float(SEARCH_BOX_SIZE)) && ball.ballContour.size()>20)
				break;
			// find the next point
			Vec2 next = current+box.getPixel(currentDir);
			// if the point is out of the ball
			if (getMask(next,ball)<=BALL_MARGIN)
			{
				bool found=false;
				for (int j=currentDir;j<currentDir+SEARCH_BOX_SIZE*6;j++)
				{
					next = current+box.getPixel(j);
					if(getMask(next,ball)>BALL_MARGIN)
					{
						currentDir=j-1;
						found=true;
						break;
					}
				}
				if (!found)
				{
					ball.ballFound=false;
					return;
				}
			}
			// if the point is in the ball
			else if (getMask(next,ball)>BALL_MARGIN)
			{
				bool found=false;
				for (int j=currentDir;j>currentDir-SEARCH_BOX_SIZE*6;j--)
				{
					next = current+box.getPixel(j);
					if(getMask(next,ball)<=BALL_MARGIN)
					{
						currentDir=j;
						found=true;
						break;
					}
				}
				if (!found)
				{
					ball.ballFound=false;
					return;
				}
			}
			// accurate sub-pixel position
			current.x=floor(current.x);
			current.y=floor(current.y);

			Vec2 sub1=current+box.getPixel(currentDir);
			Vec2 sub2=current+box.getPixel(currentDir+1);
			float mask1=getMask(sub1,ball);
			float mask2=getMask(sub2,ball);
			current=sub1+(sub2-sub1)*(BALL_MARGIN-mask1)/(mask2-mask1);
		}
		ball.position.x=0.5f*(float)(max.x+min.x);
		ball.position.y=0.5f*(float)(max.y+min.y);
		if (max.x-min.x>max.y-min.y)
			ball.ballSize=(float)(max.x-min.x);
		else
			ball.ballSize=(float)(max.y-min.y);
	}

	void ContourFinder::combImage(std::vector<MoveBall>& balls, int numBalls)
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

	bool ContourFinder::searchRow(int y, std::vector<MoveBall>& balls, int numBalls)
	{
		//search in row
		for (int x=0;x<img->w;++x)
		{
			Vec2 currentPos=Vec2(x,y);
			ColorRgb pCol=img->getPixel(currentPos);
			bool allFound=true;
			for (int i=0; i<numBalls; i++)
			{
				ColorHsv ballOut = ColorHsv(balls[i].ballOutColor);
				if (balls[i].ballFound)
					continue;
				float sim=ballOut.similarity(ColorHsv(pCol));
				if (sim>0)
				{
					balls[i].ballPerceptedColor=ColorHsv(pCol);
					balls[i].position=currentPos;
					// check if there is much similarity in the first 10 pixels
					for (int j=0; j<10; j++)
					{
						currentPos.x+=1;
						if (currentPos.x>=img->w)
							break;
						pCol=img->getPixel(currentPos);
						sim+=ballOut.similarity(ColorHsv(pCol));
					}
					if (sim>300)
					{
						balls[i].ballFound=true;
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

	void ContourFinder::searchEdge(MoveBall& ball)
	{
		// search for the edge
		while (getMask(ball.position,ball)>BALL_MARGIN)
		{
			ball.position.x--;
		}
		// accurate sub-pixel position
		float mask1=getMask(ball.position,ball);
		float mask2=getMask(Vec2(ball.position.x+1,ball.position.y),ball);
		ball.position.x=floor(ball.position.x);
		ball.position.y=floor(ball.position.y);
		ball.position.x+=(BALL_MARGIN-mask1)/(mask2-mask1);
	}

	float ContourFinder::getMask(Vec2 pos, MoveBall& ball)
	{
		if(pos.x>=img->w || pos.y>=img->h || pos.x<0 || pos.y<0)
			return 0.0f;

		unsigned short& mask=ball.mask[(int)pos.y*img->w+(int)pos.x];
		if (mask>0)
			return (float)mask/65535.0f;

		ColorRgb pixelColor=img->getPixel(pos);
		ColorHsv pixelHsv=ColorHsv(pixelColor);

		float hdiff = abs(ball.ballPerceptedColor.h-pixelHsv.h);
		if (360-hdiff<hdiff)
			hdiff=360-hdiff;

		float maskv=(pixelHsv.v-(1.0f-pixelHsv.s)*0.7f)-hdiff/360.0f*0.5f;
		if (maskv>1.0f)
			maskv=1.0f;
		if (maskv<0.0f)
			maskv=0.0f;

		mask=unsigned short(maskv*65535);
		if (mask==0)
			mask=1;
		return (float)mask/65535.0f;
	}

}