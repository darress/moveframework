#pragma once

#include "IMoveManager.h"
#include "MoveCalibration.h"
#include "MoveOrientation.h"
#include "EyeController.h"
#include "MoveController.h"


namespace Move
{
	//forward decleration
	class MoveController;
	class EyeController;

	class MoveManager : public IMoveManager
	{
		EyeController* eye;

		int moveCount;
		std::vector<MoveController*> moves;

		std::vector<MoveData> moveData;

		std::list<IMoveObserver*> observers;

		volatile int FPS;

	public:
		MoveManager(void);
		~MoveManager(void);

		//initialization
		int initMoves();
		void closeMoves();
		bool initCamera(int numMoves);
		void closeCamera();

		int getNumUsedMoves();
		int getNumAllMoves();

		//observers
		void subsribe(IMoveObserver* observer);
		void unsubsribe(IMoveObserver* observer);

		//move calibration
		int pairMoves();

		IMoveController* getMove(int moveId);
		IEyeController* getEye();

		void moveUpdated(int moveId);
		void moveKeyPressed(int moveId, int keyCode);
		void moveKeyReleased(int moveId, int keyCode);

		MoveData& getMoveDataEx(int moveId);
	};
}
