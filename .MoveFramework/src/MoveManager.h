#pragma once

#include "IMoveManager.h"
#include "MoveCalibration.h"
#include "MoveOrientation.h"
#include "EyeInterface.h"
#include "MoveController.h"


namespace Move
{
	//forward decleration
	class MoveController;

	class MoveManager : public IMoveManager
	{
		Eye::EyeInterface* eyeInt;

		int moveCount;
		std::vector<MoveController*> moves;

		std::list<IMoveObserver*> observers;

		volatile int FPS;

	public:
		MoveManager(void);
		~MoveManager(void);

		//initialization
		int initMoves();
		bool initCamera(int numMoves);

		//move count
		int getMoveCount()
		{
			return moveCount;
		}

		//observers
		void subsribeMove(IMoveObserver* observer);
		void unsubsribeMove(IMoveObserver* observer);

		//move data
		Quat getOrientation(int id);
		Vec3 getAngularVelocity(int id);
		Vec3 getAngularAcceleration(int id);
		Vec3 getPosition(int id);
		Vec3 getVelocity(int id);
		Vec3 getAcceleration(int id);
		bool getButtonState(int id, int buttonId);
		bool pointingToDisplay(int id);
		Vec3 displayPosition(int id);
		int getFrameRate();
		PBYTE getEyeBuffer();
		void getEyeDimensions(int &x, int &y);

		int getTriggerValue(int id);
		void setRumble(int id, int value);

		//move calibration
		int pairMoves();
		void useMagnetometer(int id, bool value);

		bool isCalibrated(int id);
		bool startCalibration(int id);
		void endCalibration(int id);

		Eye::EyeInterface* getEye();

		void moveUpdated(int moveId, MoveData data);
		void moveKeyPressed(int moveId, int keyCode);
		void moveKeyReleased(int moveId, int keyCode);


	};
}
