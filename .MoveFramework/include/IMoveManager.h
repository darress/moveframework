#pragma once

#include "MovePrecompiled.h"
#include "IMoveObserver.h"
#include "IMoveCalibrationObserver.h"

namespace Move
{

	class MOVE_EXPORT IMoveManager
	{
	public:
		//initialization
		virtual int initMoves()=0;
		virtual void initCamera(int numMoves)=0;

		//observers
		virtual void subsribeMove(IMoveObserver* observer)=0;
		virtual void unsubsribeMove(IMoveObserver* observer)=0;

		virtual void subsribeCalibration(IMoveCalibrationObserver* observer)=0;
		virtual void unsubsribeCalibration(IMoveCalibrationObserver* observer)=0;

		//move data
		virtual Quaternion getOrientation(int id)=0;
		virtual Vector3 getAngularVelocity(int id)=0;
		virtual Vector3 getAngularAcceleration(int id)=0;
		virtual Vector3 getPosition(int id)=0;
		virtual Vector3 getVelocity(int id)=0;
		virtual Vector3 getAcceleration(int id)=0;
		virtual bool getButtonState(int id, int buttonId)=0;
		virtual bool pointingToDisplay(int id)=0;
		virtual Vector3 displayPosition(int id)=0;
		virtual int getFrameRate(){return 0;}
		virtual PBYTE getEyeBuffer(){return 0;}
		virtual void getEyeDimensions(int &x, int &y){}

		//move calibration
		virtual bool isCalibrated(int id)=0;
		virtual bool pairMoves(){return false;}
		virtual bool startCalibration(int id){return false;}
		virtual void endCalibration(int id){}

		//events for emulator
		virtual void processKeyInput(int key){}
		//virtual void PassKeyPressed(const OIS::KeyEvent &arg){}
		//virtual void PassKeyReleased(const OIS::KeyEvent &arg){}
		virtual void passMouseMoved( float x, float y, float z ){}
		//virtual void PassWindowParam(int width, int height){}
	};
}