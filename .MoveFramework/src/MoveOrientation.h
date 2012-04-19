#pragma once

#include "MoveCalibration.h"
#include "MadgwickAHRS.h"
#include "MoveIncludes.h"
#include "PredictionFilter.h"

namespace Move
{
	class MoveCalibration;

	class MoveOrientation
	{
		int moveId;

		Quat orientation;
		Vec3 angularVel;
		Vec3 angularAcc;

		MoveCalibration* calibration;

		bool useMagnetometer;

		Madgwick::AHRS ahrs;

		float gain;
		bool highGain;

		float AEq_1, AEq_2, AEq_3, AEq_4;  // Quat orientation of earth frame relative to auxiliary frame
		PredictionFilter f1;
		PredictionFilter f2;
		PredictionFilter f3;
		PredictionFilter f4;

	public:
		MoveOrientation(int moveId);
		~MoveOrientation(void);
		void Update(Vec3 acc, Vec3 gyro, Vec3 mag, float deltat);
		Quat GetOrientation();
		Vec3 GetAngularVelocity();
		Vec3 GetAngularAcceleration();
		void UseMagnetometer(bool value);
		void Reset();
		void calibrateMagnetometer();

		void setOrientationGain(float gain);
		void useHighGain(bool use);
	};
}
