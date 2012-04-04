#pragma once

#include "IMoveObserver.h"

namespace Move
{

	class MOVE_EXPORT IMoveManager
	{
	public:
		//initialization
		virtual int initMoves()=0;
		virtual bool initCamera(int numMoves)=0;

		//observers
		virtual void subsribeMove(IMoveObserver* observer)=0;
		virtual void unsubsribeMove(IMoveObserver* observer)=0;

		//move data
		virtual Quat getOrientation(int id)=0;
		virtual Vec3 getAngularVelocity(int id)=0;
		virtual Vec3 getAngularAcceleration(int id)=0;
		virtual Vec3 getPosition(int id)=0;
		virtual Vec3 getVelocity(int id)=0;
		virtual Vec3 getAcceleration(int id)=0;
		virtual bool getButtonState(int id, int buttonId)=0;
		virtual int getTriggerValue(int id)=0;
		virtual void setRumble(int id, int value)=0;
		virtual bool pointingToDisplay(int id)=0;
		virtual Vec3 displayPosition(int id)=0;
		virtual int getFrameRate()=0;
		virtual unsigned char* getEyeBuffer()=0;
		virtual void getEyeDimensions(int &x, int &y)=0;

		//misc
		virtual int pairMoves()=0;
		virtual void useMagnetometer(int id, bool value)=0;

		//move calibration
		virtual bool isCalibrated(int id)=0;
		virtual bool startCalibration(int id)=0;
		virtual void endCalibration(int id)=0;
	};
}