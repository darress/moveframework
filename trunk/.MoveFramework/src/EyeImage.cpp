#include "EyeImage.h"
#include "MoveExceptions.h"
#include "MoveColors.h"

namespace Move
{


	EyeImage::EyeImage(int w, int h)
		:w(w),h(h)
	{
		data = new BYTE[w*h*4];
	}


	EyeImage::~EyeImage(void)
	{
		delete[] data;
	}

	void EyeImage::setPixel(Vec2 pos, ColorRgb col)
	{
		if(pos.x>=w || pos.y>=h || pos.x<0 || pos.y<0)
			throw MoveOutOfImageRangeException();
		data[(int)pos.y*w*4+(int)pos.x*4+0]=col.b; // B
		data[(int)pos.y*w*4+(int)pos.x*4+1]=col.g; // G
		data[(int)pos.y*w*4+(int)pos.x*4+2]=col.r; // R
	}

	ColorRgb EyeImage::getPixel(Vec2 pos)
	{
		ColorRgb col;
		if(pos.x>=w || pos.y>=h || pos.x<0 || pos.y<0)
		{
			throw MoveOutOfImageRangeException();
		}
		col.b=data[(int)pos.y*w*4+(int)pos.x*4+0]; // B
		col.g=data[(int)pos.y*w*4+(int)pos.x*4+1]; // G
		col.r=data[(int)pos.y*w*4+(int)pos.x*4+2]; // R
		return col;
	}

	void EyeImage::drawCircle(Vec2 center, int r, ColorRgb col)
	{
		Vec2 pix;
		float d; //detail
		for(d=0; d<=2*Move::PI; d+=0.01f) //you can play with the value to be added to d
		{
			pix.x=center.x+sin(d)*r;
			pix.y=center.y+sin(d+(Move::PI/2))*r;
			if(!(pix.x>=w || pix.y>=h || pix.x<0 || pix.y<0))
				setPixel(pix,col);

		}
	}
}