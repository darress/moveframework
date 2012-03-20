#pragma once

#include "MovePrecompiled.h"
#include "MoveCalibration.h"
#include "MadgwickAHRS.h"

namespace Move
{
	class MoveCalibration;

	class MoveOrientation
	{
		Quaternion orientation;
		Vector3 angularVel;
		Vector3 angularAcc;

		MoveCalibration* calibration;

		bool useMagnetometer;

		Madgwick::AHRS ahrs;

		CRITICAL_SECTION criticalSection;

		float AEq_1, AEq_2, AEq_3, AEq_4;  // quaternion orientation of earth frame relative to auxiliary frame

	public:
		MoveOrientation(MoveCalibration* calib);
		~MoveOrientation(void);
		void Update(Vector3 acc, Vector3 gyro, Vector3 mag, float deltat);
		Quaternion GetOrientation();
		Vector3 GetAngularVelocity();
		Vector3 GetAngularAcceleration();
		void UseMagnetometer(bool value);
		void Reset();
	};
}
