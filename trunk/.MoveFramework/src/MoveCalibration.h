#pragma once

#include "MoveRawCalibration.h"
#include "MoveManager.h"
#include "Mat3.h"
#include "MoveIncludes.h"
#include "MoveOrientation.h"

#define SWAP_MIN(min,value) if(min>value) min=value
#define SWAP_MAX(max,value) if(max<value) max=value

#define MAG_ITERATIONS 100

namespace Move
{
	struct MoveCalibrationData
	{
		Mat3 gyroGain;

		Vec3 accBias;
		Mat3 accGain;

		Vec3 magBias;
		Mat3 magGain;
	};

	class MoveOrientation;

	class MoveCalibration
	{
		enum CalibrationState
		{
			NotCalibrated,
			InitialCalibrationDone,
			MagnetometerSphereCalibration,
			MagnetometerRefCalibration,
			Calibrated,
		};

	private:
		int id;
		char deviceName[25];

		CalibrationState state;

		Vec3 magBuf[MAG_ITERATIONS];
		Quat magRef[MAG_ITERATIONS];
		Vec3 North;
		int magPosition;
		float timer;

		MoveCalibrationData data;	
		MoveRawCalibration* rawData;
		MoveOrientation* orientation;

	public:
		MoveCalibration(int id, MoveOrientation* orientation);
		//MoveCalibration(char btMac[18]);
		~MoveCalibration(void);

		void Update(Vec3 mag, Quat ref, float deltat);
		MoveCalibrationData getCalibrationData();

		bool isCalibrated(){return state!=NotCalibrated;}
		bool isMangetometerCalibrated(){return state==Calibrated;}
		void initialCalibration();
		void calibrateMagnetometer();
	private:		
		
		double integrateMagSphereError(std::vector<double> x);
		double integrateMagRefError(std::vector<double> x);
		double integrateAccError(std::vector<double> x);
		double integrateGyroError(std::vector<double> x);
	};

}
