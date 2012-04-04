#pragma once
#include "Vec3.h"
#include "MoveDevice.h"


namespace Move
{
	class MoveRawCalibration
	{
	public:
		MoveRawCalibration(MoveDevice::TMoveCalib calib);
		~MoveRawCalibration(void);

	public:
		short int AccHeader;
		Vec3 AccVectors[6];
		short int GyroBiasHeaders[2];
		Vec3 GyroBiasVectors[2];
		short int GyroHeader;
		Vec3 GyroVectors[3];
		short int UnknownHeader;
		Vec3 UnknownVectors[2];
		float UnknownValues[2];
	};

}