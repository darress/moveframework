#pragma once
#include "MoveManager.h"
#include "MoveOrientation.h"
#include "MoveCalibration.h"
#include "MoveData.h"
#include "EyeInterface.h"

namespace Move
{
	//forward decleration
	class MoveManager;
	class MoveOrientation;
	class MoveCalibration;

	class MoveController
	{
		HANDLE _hThread;

		int id;
		MoveManager* manager;

		MoveOrientation* orientation;
		MoveCalibration* calibration;

		int lastSeqNumber;
		int lastTimestamp;

		bool firstPackage;
		float resetTimer;

		static float offsetX,offsetY,offsetZ;
	public:
		MoveData data;

		MoveController(int p_id, MoveManager* p_manager);
		~MoveController(void);

		void Update();
		bool isCalibrated();
		bool StartCalibration();
		void EndCalibration();
		void UseMagnetometer(bool value);
		void setRumble(int value);

	private:
		static DWORD WINAPI controlThread(LPVOID instance);
	};
}