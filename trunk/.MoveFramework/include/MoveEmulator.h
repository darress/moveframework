#pragma once
#include "IMoveManager.h"
#include "MovePrecompiled.h"

namespace Move
{
	class MoveEmulator : public IMoveManager
	{
		enum Mode {READ_LOG,WRITE_LOG,CONTROLLER};
		Mode EmuMode;

		int WindowH;
		int WindowW;

		std::queue<Quaternion> orientations;
		Vector3 move_position;
		Vector3 player_position;
		float roll;
		float pitch;
		float yaw;

		float player_roll;
		float player_pitch;
		float player_yaw;

		float EmulatorSpeed;

		//bool KeyMap[3][2];

		std::list<IMoveObserver*> observers;
		std::list<IMoveCalibrationObserver*> calibrationObservers;

		std::map<int, int> keyMap;

	public:
		MoveEmulator(void);
		~MoveEmulator(void);

		//initialization
		int initMoves();
		void initCamera(int numMoves);

		//observers
		void subsribeMove(IMoveObserver* observer);
		void unsubsribeMove(IMoveObserver* observer);

		void subsribeCalibration(IMoveCalibrationObserver* observer);
		void unsubsribeCalibration(IMoveCalibrationObserver* observer);

		//move data
		Quaternion getOrientation(int id);
		Vector3 getAngularVelocity(int id);
		Vector3 getAngularAcceleration(int id);
		Vector3 getPosition(int id);
		Vector3 getVelocity(int id);
		Vector3 getAcceleration(int id);
		bool getButtonState(int id, int buttonId);
		bool pointingToDisplay(int id);
		Vector3 displayPosition(int id);

		//events for emulator
		virtual void processKeyInput(int key);
		//void PassKeyPressed(const OIS::KeyEvent &arg);
		//void PassKeyReleased(const OIS::KeyEvent &arg);
		void passMouseMoved( float x, float y, float z );
		//void PassWindowParam(int width, int height);

		bool isCalibrated(int id) {return true; }
	};
}

