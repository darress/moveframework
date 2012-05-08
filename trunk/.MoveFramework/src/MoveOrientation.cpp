#include "MoveOrientation.h"
#include "IniFile.h"

namespace Move
{
	MoveOrientation::MoveOrientation(int moveId)
		:moveId(moveId)
	{
		highGain=false;

		gain=ahrs.beta;
		try
		{
			setOrientationGain( IniFile::GetFloat("AHRSalgorithmGain", "Tracking", "settings.cfg") );
		}
		catch(MoveConfigFileRecordNotFoundException)
		{}

		calibration = new MoveCalibration(moveId, this);

		if (calibration->isMangetometerCalibrated())
			useMagnetometer=true;
		else
			useMagnetometer=false;

		try
		{
			int predictionBufferSize = IniFile::GetInt("PredictionBufferSize", "Tracking", "settings.cfg");
			f1=PredictionFilter(predictionBufferSize);
			f2=PredictionFilter(predictionBufferSize);
			f3=PredictionFilter(predictionBufferSize);
			f4=PredictionFilter(predictionBufferSize);
		}
		catch(MoveConfigFileRecordNotFoundException)
		{}

		angularVel=Vec3(0,0,0);
		angularAcc=Vec3(0,0,0);

		AEq_1 = 1, AEq_2 = 0, AEq_3 = 0, AEq_4 = 0;

		try
		{
			useMagnetometer = IniFile::GetInt("UseMagnetometers", "Tracking", "settings.cfg")!=0;
		}
		catch(MoveConfigFileRecordNotFoundException)
		{}
	}


	MoveOrientation::~MoveOrientation(void)
	{
		delete calibration;
	}

	void MoveOrientation::UseMagnetometer(bool value)
	{
		if (useMagnetometer!=value && calibration->isMangetometerCalibrated())
		{
			useMagnetometer=value;
			if (value)
				MoveManager::getInstance()->notify(moveId, M_UseMagnetometers);
			else
				MoveManager::getInstance()->notify(moveId, M_DoesntUseMagnetometers);
		}
	}

	void MoveOrientation::Update(Vec3 acc, Vec3 gyro, Vec3 mag, float deltat)
	{
		Vec3 oldMag=mag;
		if (!calibration->isCalibrated())
			calibration->initialCalibration();

		if (deltat==0.0f) return;

		MoveCalibrationData calib = calibration->getCalibrationData();

		acc=(acc-calib.accBias)*calib.accGain;
		gyro=gyro*calib.gyroGain;

		if (useMagnetometer && (mag.x!=0 || mag.y!=0 || mag.z!=0))
		{
			mag=(mag-calib.magBias)*calib.magGain;
			ahrs.MadgwickAHRSupdate(gyro.x,gyro.y,gyro.z,acc.x,acc.y,acc.z,mag.x,mag.y,mag.z,deltat);
		}
		else
		{
			ahrs.MadgwickAHRSupdateIMU(gyro.x,gyro.y,gyro.z,acc.x,acc.y,acc.z,deltat);
		}

		// calibration
		Quat frameOri=Quat(ahrs.q0,ahrs.q1,ahrs.q2,ahrs.q3);
		calibration->Update(oldMag,frameOri,deltat);

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

		orientation = Quat(ASq_1,ASq_2,ASq_3,-ASq_4);
		orientation.Normalize();

		orientation.w=f1.filter(orientation.w,deltat);
		orientation.v.x=f2.filter(orientation.v.x,deltat);
		orientation.v.y=f3.filter(orientation.v.y,deltat);
		orientation.v.z=f4.filter(orientation.v.z,deltat);
		
		angularAcc=(gyro-angularVel)/deltat;
		angularVel=gyro;
	}

	Quat MoveOrientation::GetOrientation()
	{
		return orientation;
	}

	Vec3 MoveOrientation::GetAngularVelocity()
	{
		return angularVel;
	}

	Vec3 MoveOrientation::GetAngularAcceleration()
	{
		return angularAcc;
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

	void MoveOrientation::setOrientationGain(float gain)
	{
		this->gain=gain;
		if (!highGain)
			ahrs.beta=gain;
	}

	void MoveOrientation::useHighGain(bool use)
	{
		highGain=use;
		if (highGain)
			ahrs.beta=5.0f;
		else
			ahrs.beta=gain;
	}

	void MoveOrientation::calibrateMagnetometer()
	{
		calibration->calibrateMagnetometer();
	}
}