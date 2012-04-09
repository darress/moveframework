#pragma once

#include "Vec2.h"
#include "MoveBall.h"
#include "MoveColors.h"
#include "EyeImage.h"

namespace Move
{
	//ball algorithm constants
	const float BALL_MARGIN=0.3f;

	class ContourFinder
	{
	private:
		EyeImage* img;

	public:
		ContourFinder(EyeImage* img);
		~ContourFinder();

		/**
		 * Searches for the balls, then search for their contour and store it in the MoveBall struct.
		 * @param balls The balls.
		 * @param numBalls Number of balls.
		 */
		void findBalls(std::vector<MoveBall>& balls, int numBalls);
		float getMask(Vec2 pos, MoveBall& ball);

	private:
		/**
		 * Comb the image for the balls, and store the first found pixel of the ball in the MoveBall.position member.
		 * The function ignores the balls if MoveBall.ballFound is true
		 * @param balls The balls.
		 * @param numBalls Number of balls.
		 */
		void combImage(std::vector<MoveBall>& balls, int numBalls);

		/**
		 * Searches in a single row for the balls.
		 * @param y The y coordinate of the row.
		 * @param [in,out] balls The balls.
		 * @param numBalls Number of balls.
		 * @return true if it find all the balls.
		 */
		bool searchRow(int y, std::vector<MoveBall>& balls, int numBalls);

		void findContour(MoveBall& ball);

		void searchEdge(MoveBall& ball);

		struct PixelBox
		{
		private:
			int size;

		public:
			PixelBox(int size):size(size)
			{
			}

			Vec2 getPixel(int num)
			{
				num=num % (8*size);
				if (num<0)
					num+=8*size;
				int side=num/(2*size);
				int subNum;
				if (side<2)
					subNum=(num % (size*2))-size;
				else
					subNum=size-(num % (size*2));
				if (side==0)
					return Vec2(subNum,-size);
				else if (side==1)
					return Vec2(size,subNum);
				else if (side==2)
					return Vec2(subNum,size);
				else if (side==3)
					return Vec2(-size,subNum);
			}
		};
	};

}