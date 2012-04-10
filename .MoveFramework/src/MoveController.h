#pragma once
#include "MoveManager.h"
#include "MoveOrientation.h"
#include "MoveCalibration.h"
#include "MoveData.h"
#include "EyeController.h"
#include "IMoveController.h"

namespace Move
{
	//forward decleration
	class MoveManager;
	class MoveOrientation;

	class MoveController : public IMoveController
	{
		HANDLE _hThread;

		int id;
		MoveManager* manager;

		MoveOrientation* orientation;

		int lastSeqNumber;
		int lastTimestamp;

		bool firstPackage;
		float resetTimer;

		static float offsetX,offsetY,offsetZ;
	public:

		MoveController(int p_id, MoveManager* p_manager);
		~MoveController(void);

		void Update();
		void useMagnetometers(bool value);
		void setRumble(int value);
		void setOrientationGain(float gain);
		MoveData getMoveData();

	private:
		static DWORD WINAPI controlThread(LPVOID instance);
	};
}