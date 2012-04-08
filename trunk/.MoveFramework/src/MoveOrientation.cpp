#include "MoveOrientation.h"
#include "IniFile.h"

namespace Move
{
	MoveOrientation::MoveOrientation(MoveCalibration* calib)
	{
		InitializeCriticalSection(&criticalSection);

		calibration=calib;

		useMagnetometer=true;

		angularVel=Vec3(0,0,0);
		angularAcc=Vec3(0,0,0);

		AEq_1 = 1, AEq_2 = 0, AEq_3 = 0, AEq_4 = 0;
		try
		{
			ahrs.beta = IniFile::GetFloat("AHRSalgorithmGain", "Tracking", "settings.cfg");
		}
		catch(MoveConfigFileRecordNotFoundException)
		{}
	}


	MoveOrientation::~MoveOrientation(void)
	{
		DeleteCriticalSection(&criticalSection);
	}

	void MoveOrientation::UseMagnetometer(bool value)
	{
		useMagnetometer=value;
	}

	void MoveOrientation::Update(Vec3 acc, Vec3 gyro, Vec3 mag, float deltat)
	{
		if (deltat==0.0f) return;

		MoveCalibrationData calib = calibration->getCalibrationData();

		acc=(acc-calib.accBias)*calib.accGain;
		gyro=gyro*calib.gyroGain;

		if (useMagnetometer && (mag.x!=0 || mag.y!=0 || mag.z!=0))
		{
			mag=calib.magGain*(mag-calib.magBias);
			ahrs.MadgwickAHRSupdate(gyro.x,gyro.y,gyro.z,acc.x,acc.y,acc.z,mag.x,mag.y,mag.z,deltat);
		}
		else
		{
			ahrs.MadgwickAHRSupdateIMU(gyro.x,gyro.y,gyro.z,acc.x,acc.y,acc.z,deltat);
		}

		float ESq_1, ESq_2, ESq_3, ESq_4;                              // Quat describing orientation of sensor relative to earth

		// compute the Quat conjugate
		ESq_1 = ahrs.q0;
		ESq_2 = -ahrs.q1;
		ESq_3 = -ahrs.q2;
		ESq_4 = -ahrs.q3;

		float ASq_1, ASq_2, ASq_3, ASq_4;

		// compute the Quat product
		ASq_1 = ESq_1 * AEq_1 - ESq_2 * AEq_2 - ESq_3 * AEq_3 - ESq_4 * AEq_4;
		ASq_2 = ESq_1 * AEq_2 + ESq_2 * AEq_1 + ESq_3 * AEq_4 - ESq_4 * AEq_3;
		ASq_3 = ESq_1 * AEq_3 - ESq_2 * AEq_4 + ESq_3 * AEq_1 + ESq_4 * AEq_2;
		ASq_4 = ESq_1 * AEq_4 + ESq_2 * AEq_3 - ESq_3 * AEq_2 + ESq_4 * AEq_1;

		EnterCriticalSection(&criticalSection);
			orientation = Quat((float)ASq_1,(float)ASq_2,(float)ASq_3,-(float)ASq_4);
			angularAcc=(gyro-angularVel)/deltat;
			angularVel=gyro;
		LeaveCriticalSection(&criticalSection);
	}

	Quat MoveOrientation::GetOrientation()
	{
		EnterCriticalSection(&criticalSection);
		return orientation;
		LeaveCriticalSection(&criticalSection);
	}

	Vec3 MoveOrientation::GetAngularVelocity()
	{
		EnterCriticalSection(&criticalSection);
		return angularVel;
		LeaveCriticalSection(&criticalSection);
	}

	Vec3 MoveOrientation::GetAngularAcceleration()
	{
		EnterCriticalSection(&criticalSection);
		return angularAcc;
		LeaveCriticalSection(&criticalSection);
	}

	void MoveOrientation::Reset()
	{
		//useMagnetometer=true;
		// store orientation of auxiliary frame
		AEq_1 = ahrs.q0;                                                              
		AEq_2 = ahrs.q1;
		AEq_3 = ahrs.q2;
		AEq_4 = ahrs.q3;
	}
}