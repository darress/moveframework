#include "MovePrecompiled.h"
#include "MoveOrientation.h"
#include "MadgwickAHRS.h"

namespace Move
{
	MoveOrientation::MoveOrientation(MoveCalibration* calib)
	{
		InitializeCriticalSection(&criticalSection);

		calibration=calib;

		useMagnetometer=true;

		angularVel=Vector3(0,0,0);
		angularAcc=Vector3(0,0,0);

		AEq_1 = 1, AEq_2 = 0, AEq_3 = 0, AEq_4 = 0;
	}


	MoveOrientation::~MoveOrientation(void)
	{
		DeleteCriticalSection(&criticalSection);
	}

	void MoveOrientation::UseMagnetometer(bool value)
	{
		useMagnetometer=value;
	}

	void MoveOrientation::Update(Vector3 acc, Vector3 gyro, Vector3 mag, float deltat)
	{
		if (deltat==0.0f) return;

		MoveCalibrationData calib = calibration->getCalibrationData();

		acc=calib.accGain*(acc-calib.accBias);
		gyro=calib.gyroGain*gyro;

		if (useMagnetometer && (mag.x!=0 || mag.y!=0 || mag.z!=0))
		{
			mag=calib.magGain*(mag-calib.magBias);
			Madgwick::MadgwickAHRSupdate(gyro.x,gyro.y,gyro.z,acc.x,acc.y,acc.z,mag.x,mag.y,mag.z,deltat);
		}
		else
		{
			Madgwick::MadgwickAHRSupdateIMU(gyro.x,gyro.y,gyro.z,acc.x,acc.y,acc.z,deltat);
		}

		float ESq_1, ESq_2, ESq_3, ESq_4;                              // quaternion describing orientation of sensor relative to earth

		// compute the quaternion conjugate
		ESq_1 = Madgwick::q0;
		ESq_2 = -Madgwick::q1;
		ESq_3 = -Madgwick::q2;
		ESq_4 = -Madgwick::q3;

		float ASq_1, ASq_2, ASq_3, ASq_4;

		// compute the quaternion product
		ASq_1 = ESq_1 * AEq_1 - ESq_2 * AEq_2 - ESq_3 * AEq_3 - ESq_4 * AEq_4;
		ASq_2 = ESq_1 * AEq_2 + ESq_2 * AEq_1 + ESq_3 * AEq_4 - ESq_4 * AEq_3;
		ASq_3 = ESq_1 * AEq_3 - ESq_2 * AEq_4 + ESq_3 * AEq_1 + ESq_4 * AEq_2;
		ASq_4 = ESq_1 * AEq_4 + ESq_2 * AEq_3 - ESq_3 * AEq_2 + ESq_4 * AEq_1;

		EnterCriticalSection(&criticalSection);
			orientation = Quaternion((float)ASq_1,(float)ASq_2,(float)ASq_3,-(float)ASq_4);
			angularAcc=(gyro-angularVel)/deltat;
			angularVel=gyro;
		LeaveCriticalSection(&criticalSection);
	}

	Quaternion MoveOrientation::GetOrientation()
	{
		EnterCriticalSection(&criticalSection);
		return orientation;
		LeaveCriticalSection(&criticalSection);
	}

	Vector3 MoveOrientation::GetAngularVelocity()
	{
		EnterCriticalSection(&criticalSection);
		return angularVel;
		LeaveCriticalSection(&criticalSection);
	}

	Vector3 MoveOrientation::GetAngularAcceleration()
	{
		EnterCriticalSection(&criticalSection);
		return angularAcc;
		LeaveCriticalSection(&criticalSection);
	}

	//only for testing
	void MoveOrientation::HighGains()
	{
		Madgwick::beta = 0.5f;
		useMagnetometer=false;
		// store orientation of auxiliary frame
		AEq_1 = Madgwick::q0;                                                              
		AEq_2 = Madgwick::q1;
		AEq_3 = Madgwick::q2;
		AEq_4 = Madgwick::q3;

	}

	void MoveOrientation::Reset()
	{
		//Madgwick::beta = 0.5f;
		//useMagnetometer=true;
		// store orientation of auxiliary frame
		AEq_1 = Madgwick::q0;                                                              
		AEq_2 = Madgwick::q1;
		AEq_3 = Madgwick::q2;
		AEq_4 = Madgwick::q3;
	}
}