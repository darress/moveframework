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

		static MoveManager* instance;

	private:
		MoveManager();
	public:
		~MoveManager();

		static MoveManager* getInstance()
		{
			if (instance==0)
				instance=new MoveManager();
			return instance;
		}

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
		void moveKeyPressed(int moveId, MoveButton button);
		void moveKeyReleased(int moveId, MoveButton button);

		void notify(int moveId, MoveMessage message);

		MoveData& getMoveDataEx(int moveId);
	};
}
