#include "BallColorManager.h"
#include "IniFile.h"

#define TIMER_INIT 10

namespace Move
{
	ColorRgb BallColorManager::colors[]={ColorRgb(255,0,0),ColorRgb(255,255,0),ColorRgb(0,255,0),ColorRgb(0,255,255),ColorRgb(0,0,255),ColorRgb(255,0,255)};

	BallColorManager::BallColorManager(EyeImage* image, std::vector<MoveBall*>& balls)
		:image(image), balls(balls)
	{
		calibrated=0;
		currentColor=0;
		timer=TIMER_INIT;
		automaticColors=true;
		ballColors.resize(balls.size());

		for (int i=0; i<balls.size(); i++)
		{
			char tmp[20];
			sprintf_s(tmp,"BallColor%d",i);
			try
			{
				Vec3 ballColor=IniFile::GetVec3(tmp, "Tracking", "settings.cfg");
				automaticColors=false;
				balls[i]->ballOutColor=ColorRgb((int)ballColor.x, (int)ballColor.y, (int)ballColor.z);

			}catch(MoveConfigFileRecordNotFoundException){}
		}
	}


	BallColorManager::~BallColorManager()
	{
	}

	void BallColorManager::calculateColors()
	{
		if (!automaticColors)
			return;
		if (calibrated==balls.size())
		{
			for (int i=0; i<balls.size(); i++)
			{
				balls[i]->ballOutColor=colors[ballColors[i]];
			}
			return;
		}

		for (int i=0; i<balls.size(); i++)
		{
			balls[i]->ballOutColor=ColorRgb(0,0,0);
		}

		if (usedColors.count(currentColor)>0)
		{
			currentColor++;
			if (currentColor>=NUM_COLORS)
			{
				findBestColor();
			}
			return;
		}

		balls[calibrated]->ballOutColor=colors[currentColor];
				
		if (balls[calibrated]->ballFound)
		{
			if (--timer>0)
				return;
			// calculating error
			float error=0;
			for (int ii=0; ii<640; ii++)
			{
				for (int j=0; j<480; j++)
				{
					Vec2 current=Vec2(ii,j);
					if ((current-balls[calibrated]->position).length()>balls[calibrated]->ballSize/2.0f)
						error+=balls[calibrated]->getMask(current);
				}
			}
			colorError[currentColor]=error;
			// jump to next color, init the timer
			currentColor++;
			timer=TIMER_INIT;
			// if all colors were examined, find the best
			if (currentColor>=NUM_COLORS)
			{
				findBestColor();
			}
		}
	}

	void BallColorManager::findBestColor()
	{
		int bestNum=-1;
		float best;
		for (int i=0;i<NUM_COLORS;i++)
		{
			if (usedColors.count(i)>0)
				continue;
			if (bestNum==-1 || best>colorError[i])
			{
				best=colorError[i];
				bestNum=i;
			}
		}
		balls[calibrated]->ballOutColor=colors[bestNum];
		usedColors.insert(bestNum);
		// insert the color before the used (to ensure the colors are not similar)
		if (bestNum==0)
			usedColors.insert(NUM_COLORS-1);
		else
			usedColors.insert(bestNum-1);
		// insert the color next to the used (to ensure the colors are not similar)
		if (bestNum==NUM_COLORS-1)
			usedColors.insert(0);
		else
			usedColors.insert(bestNum+1);
		// store the color
		ballColors[calibrated]=bestNum;
		// jump to the next ball
		calibrated++;
		// reset veriables for next turn
		currentColor=0;
		timer=TIMER_INIT;
	}

	void BallColorManager::useAutomaticColors(bool use)
	{
		automaticColors=use;
	}

	void BallColorManager::setColor(MoveBall* ball, int r, int g, int b)
	{
		if (!automaticColors)
		{
			ball->ballOutColor=ColorRgb(r,g,b);
		}
	}

}