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
		int id;
		MoveManager* manager;

		MoveOrientation* orientation;
		MoveCalibration* calibration;

		Vector3 estimatedVelocity;
		Vector3 estimatedPosition;

		int lastSeqNumber;
		int lastTimestamp;

		int packageGot;
		int missingPackage;

		bool firstPackage;

	public:
		MoveData data;

		MoveController(int p_id, MoveManager* p_manager);
		~MoveController(void);

		void Update();
		bool isCalibrated();
		bool StartCalibration();
		void EndCalibration();
	};
}