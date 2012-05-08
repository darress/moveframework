#include "MoveBall.h"

namespace Move
{

	MoveBall::MoveBall(EyeImage* img)
		:img(img)
	{
		ballSize=0;
		ballFound=false;
		mask = new unsigned short[img->w*img->h*2];
	}


	MoveBall::~MoveBall()
	{
		delete[] mask;
	}

	float MoveBall::getMask(Vec2 pos)
	{
		if(pos.x>=img->w || pos.y>=img->h || pos.x<0 || pos.y<0)
			return 0.0f;

		unsigned short& curmask=mask[(int)pos.y*img->w+(int)pos.x];
		if (curmask>0)
			return (float)curmask/65535.0f;

		ColorRgb pixelColor=img->getPixel(pos);
		ColorHsv pixelHsv=ColorHsv(pixelColor);

		float hdiff = abs(ballPerceptedColor.h-pixelHsv.h);
		if (360-hdiff<hdiff)
			hdiff=360-hdiff;

		float maskv=(pixelHsv.v-(1.0f-pixelHsv.s)*0.7f)-hdiff/360.0f*0.5f;
		if (maskv>1.0f)
			maskv=1.0f;
		if (maskv<0.0f)
			maskv=0.0f;

		curmask=unsigned short(maskv*65535);
		if (curmask==0)
			curmask=1;
		return (float)curmask/65535.0f;
	}

	void MoveBall::showProcessedPixels()
	{
		int a=0;
		for (int i=0; i<640; i++)
		{
			for (int j=0; j<480; j++)
			{
				Vec2 pos=Vec2(i,j);
				unsigned short& curmask=mask[(int)pos.y*img->w+(int)pos.x];
				if (curmask>0)
				{
					curmask=65535;
					a++;
				}
			}
		}
		int b=4;
	}

	void MoveBall::resetMask()
	{
		memset(mask,0,img->w*img->h*2);
	}

	unsigned char* MoveBall::getMaskBuffer()
	{
		return (unsigned char*)mask;
	}
}