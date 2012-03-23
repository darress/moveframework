#include "MovePrecompiled.h"
#include "EyeImage.h"
#include "Helpers.h"
#include "simplex.h"

namespace Eye
{
	std::vector<glm::vec2> EyeImage::currentBallContour;
	float EyeImage::currentBallSize;

	EyeImage::EyeImage(int w, int h, PBYTE data)
		:w(w),h(h),data(data)
	{
	}


	EyeImage::~EyeImage(void)
	{
	}

	void EyeImage::setPixel(Vector2 pos, ColorRgb col)
	{
		if(pos.x>=w || pos.y>=h || pos.x<0 || pos.y<0)
			throw MoveOutOfImageRangeException();
		data[pos.y*w*4+pos.x*4+0]=col.b; // B
		data[pos.y*w*4+pos.x*4+1]=col.g; // G
		data[pos.y*w*4+pos.x*4+2]=col.r; // R
	}

	ColorRgb EyeImage::getPixel(Vector2 pos)
	{
		ColorRgb col;
		if(pos.x>=w || pos.y>=h || pos.x<0 || pos.y<0)
		{
			throw MoveOutOfImageRangeException();
		}
		col.b=data[pos.y*w*4+pos.x*4+0]; // B
		col.g=data[pos.y*w*4+pos.x*4+1]; // G
		col.r=data[pos.y*w*4+pos.x*4+2]; // R
		return col;
	}

	void EyeImage::drawCircle(Vector2 center, int r, ColorRgb col)
	{
		Vector2 pix;
		float d; //detail
		for(d=0; d<=2*Move::PI; d+=0.01f) //you can play with the value to be added to d
		{
			pix.x=(int)(center.x+sin(d)*r);
			pix.y=(int)(center.y+sin(d+(Move::PI/2))*r);
			if(!(pix.x>=w || pix.y>=h || pix.x<0 || pix.y<0))
				setPixel(pix,col);

		}
	}

	void EyeImage::findBalls(Ball* balls, int numBalls)
	{
		bool needToComb = false;
		for (int i=0; i<numBalls; i++)
		{
			if (balls[i].ballFound && pixelMatches(balls[i].position, balls[i].ballPerceptedColor))
			{
				while (pixelMatches(Vector2(balls[i].position.x-1,balls[i].position.y), balls[i].ballPerceptedColor))
				{
					balls[i].position.x--;
				}
			}
			else
			{
				balls[i].ballFound=false;
				needToComb=true;
			}
		}
		if (needToComb)
		{
			combImage(balls, numBalls);
		}

		for (int i=0; i<numBalls; i++)
		{
			if (balls[i].ballFound)
			{
				findContour(&(balls[i]));
				if (balls[i].ballContour.size()<30)
				{
					balls[i].ballFound=false;
					continue;
				}
				fitCircle(&(balls[i]));

				if (balls[i].ballSize>15 && pixelMatches(balls[i].position, balls[i].ballPerceptedColor))
				{
					balls[i].ballPerceptedColor=ColorHsv(getPixel(balls[i].position));
				}
			}
		}
	}

	void EyeImage::fitCircle(Ball* ball)
	{
		currentBallContour.clear();
		
		int skipmax=4;
		int skip=skipmax;
		std::list<Vector2>::iterator it;
		for ( it=ball->ballContour.begin() ; it != ball->ballContour.end(); it++ )
		{
			skip--;
			if (skip==0)
				skip=skipmax;
			else
				continue;
			Vector2 p=*it;
			if (p.x>2 && p.y>1 && p.x<w-1 && p.y<h-1)
			{
				glm::vec2 pp=glm::vec2(p.x,p.y);
				currentBallContour.push_back(pp);
			}
		}

		std::vector<double> init;
		std::vector<double> result;

		init.push_back(ball->positionX);
		init.push_back(ball->positionY);
		currentBallSize= (ball->ballSize)*0.5f;

		result.clear();
		result=BT::Simplex(&Eye::EyeImage::integrateCircleError, init, 30);

		ball->positionX=(float)result[0];
		ball->positionY=(float)result[1];

		float size=0;
		int nums=0;
		glm::vec2 midp=glm::vec2(result[0],result[1]);
		for (unsigned int i=0; i<currentBallContour.size(); i++)
		{
			float dis=glm::distance(midp,currentBallContour[i]);
			if (dis>currentBallSize*0.9f)
			{
				size+=dis;
				nums++;
			}
		}
		ball->ballSize=(size/(float)nums)*2.0f;
	}

	double EyeImage::integrateCircleError(std::vector<double> x)
	{
		double error=0.0f;

		glm::vec2 midp=glm::vec2(x[0],x[1]);

		for (unsigned int i=0; i<currentBallContour.size(); i++)
		{
			float dis=glm::distance2(midp,currentBallContour[i]);
			error+=abs(dis-currentBallSize*currentBallSize);
		}

		return error;
	}

	void EyeImage::findContour(Ball* ball)
	{
		Vector2 init=ball->position;
		ball->ballContour.clear();
		Vector2 current;
		if (!checkIfContour(init, ball))
		{
			ball->ballFound=false;
			return;
		}
		ball->ballContour.push_back(init);
		setPixel(init,ColorRgb(255,0,0));
		current=findNeighbor(init, ball);
		if (current.x==-1)
		{
			ball->ballFound=false;
			return;
		}

		Vector2 min, max;
		min=max=current;
		for (int i=0;i<10000;i++)
		{
			ball->ballContour.push_back(current);
			setPixel(current,ColorRgb(255,0,0));
			if (current.x<min.x) min.x=current.x;
			if (current.y<min.y) min.y=current.y;
			if (current.x>max.x) max.x=current.x;
			if (current.y>max.y) max.y=current.y;
			Vector2 dis = Vector2(current.x-init.x,current.y-init.y);
			if (dis.x*dis.x+dis.y*dis.y<4 && ball->ballContour.size()>6)
				break;
			current = findNeighbor(current, ball);
			if (current.x==-1)
			{
				while ( !ball->ballContour.empty() )
				{
					Vector2 cnt = ball->ballContour.back();
					ball->ballContour.pop_back();
					current = findNeighbor(cnt, ball);
					if (current.x!=-1)
						break;
				}
			}
			if (current.x==-1)
				break;
			if (ball->ballContour.size()>2500)
				break;
		}
		ball->positionX=0.5f*(float)(max.x+min.x);
		ball->positionY=0.5f*(float)(max.y+min.y);
		if (max.x-min.x>max.y-min.y)
			ball->ballSize=(float)(max.x-min.x);
		else
			ball->ballSize=(float)(max.y-min.y);
	}

	Vector2 EyeImage::findNeighbor(Vector2 pos, Ball* ball)
	{
		Vector2 search;

		search=Vector2(pos.x+1,pos.y+1);
		if (checkIfContour(search, ball))
			return search;
		search=Vector2(pos.x+1,pos.y);
		if (checkIfContour(search, ball))
			return search;
		search=Vector2(pos.x+1,pos.y-1);
		if (checkIfContour(search, ball))
			return search;
		search=Vector2(pos.x,pos.y-1);
		if (checkIfContour(search, ball))
			return search;
		search=Vector2(pos.x-1,pos.y-1);
		if (checkIfContour(search, ball))
			return search;
		search=Vector2(pos.x-1,pos.y);
		if (checkIfContour(search, ball))
			return search;
		search=Vector2(pos.x-1,pos.y+1);
		if (checkIfContour(search, ball))
			return search;
		search=Vector2(pos.x,pos.y+1);
		if (checkIfContour(search, ball))
			return search;

		return Vector2(-1,-1);
	}

	bool EyeImage::checkIfContour(Vector2 pos, Ball* ball)
	{
		if(pos.x>=w || pos.y>=h || pos.x<0 || pos.y<0)
			return false;
		ColorRgb pixel;
		if (!pixelMatches(pos, ball->ballPerceptedColor))
			return false;
		if (pixelAlreadyFound(pos))
			return false;
		if (!pixelMatches(Vector2(pos.x-1,pos.y), ball->ballPerceptedColor))
			return true;
		if (!pixelMatches(Vector2(pos.x+1,pos.y), ball->ballPerceptedColor))
			return true;
		if (!pixelMatches(Vector2(pos.x,pos.y-1), ball->ballPerceptedColor))
			return true;
		if (!pixelMatches(Vector2(pos.x,pos.y+1), ball->ballPerceptedColor))
			return true;
		return false;
	}

	void EyeImage::combImage(Ball* balls, int numBalls)
	{
		float searchGap=(float)h;
		Vector2 found;
		float searchY;
		while (searchGap>4)
		{
			searchY=searchGap/2;
			while ((int)searchY<h)
			{
				if (searchRow((int)searchY, found, balls, numBalls))
					return;
				searchY+=searchGap;
			}
			searchGap/=2;
		}
	}

	bool EyeImage::searchRow(int y, Vector2& found, Ball* balls, int numBalls)
	{
		//search in row
		for (int x=0;x<w;++x)
		{
			Vector2 currentPos=Vector2(x,y);
			ColorRgb pCol=getPixel(currentPos);
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
					//check if there is much similarity in the first 10 pixels
					for (int j=0; j<10; j++)
					{
						currentPos.x+=1;
						if (currentPos.x>=w)
							break;
						pCol=getPixel(currentPos);
						sim+=ballOut.similarity(ColorHsv(pCol));
					}
					if (sim>300)
					{
						balls[i].ballFound=true;
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

	bool EyeImage::colorMatches(ColorRgb pixelColor, ColorHsv ballColor)
	{
		if (pixelColor.r==255 && pixelColor.g==0 && pixelColor.b==0)
			return true;

		ColorHsv pixelHsv=ColorHsv(pixelColor);

		//only for debug
		float x = ballColor.h;
		
		float hdiff = abs(ballColor.h-pixelHsv.h);
		if (360-hdiff<hdiff)
			hdiff=360-hdiff;

		if (pixelHsv.v>0.7 && hdiff<40)
			return true;

		return false;
	}

	bool EyeImage::pixelMatches(Vector2 pos, ColorHsv ballColor)
	{
		if(pos.x>=w || pos.y>=h || pos.x<0 || pos.y<0)
			return false;
		ColorRgb pixel;
		pixel=getPixel(pos);
		return colorMatches(pixel, ballColor);
	}

	bool EyeImage::pixelAlreadyFound(Vector2 pos)
	{
		if(pos.x>=w || pos.y>=h || pos.x<0 || pos.y<0)
			return false;
		ColorRgb pixel;
		pixel=getPixel(pos);
		if (pixel.r==255 && pixel.g==0 && pixel.b==0)
			return true;
		return false;
	}
}