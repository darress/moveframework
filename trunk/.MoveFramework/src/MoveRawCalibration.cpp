#include "MoveRawCalibration.h"
#include "IniFile.h"

using namespace std;

namespace Move
{
	MoveRawCalibration::MoveRawCalibration(MoveDevice::TMoveCalib calib)
	{
		AccHeader=calib.AccHeader;
		for (int i=0; i<6; i++)
			AccVectors[i]=Vec3(calib.AccVectors[i][0],calib.AccVectors[i][1],calib.AccVectors[i][2]);
		for (int i=0; i<2; i++)
			GyroBiasHeaders[i]=calib.GyroBiasHeaders[i];
		for (int i=0; i<2; i++)
			GyroBiasVectors[i]=Vec3(calib.GyroBiasVectors[i][0],calib.GyroBiasVectors[i][1],calib.GyroBiasVectors[i][2]);
		GyroHeader=calib.GyroHeader;
		for (int i=0; i<3; i++)
			GyroVectors[i]=Vec3(calib.GyroVectors[i][0],calib.GyroVectors[i][1],calib.GyroVectors[i][2]);
		UnknownHeader=calib.UnknownHeader;
		for (int i=0; i<2; i++)
			UnknownVectors[i]=Vec3(calib.UnknownVectors[i][0],calib.UnknownVectors[i][1],calib.UnknownVectors[i][2]);
		for (int i=0; i<2; i++)
			UnknownValues[i]=calib.UnknownValues[i];
	}

	MoveRawCalibration::~MoveRawCalibration(void)
	{
	}

}