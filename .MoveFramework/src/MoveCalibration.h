#pragma once

#include "MoveRawCalibration.h"
#include "MoveManager.h"
#include "Mat3.h"
#include "MoveIncludes.h"

#define SWAP_MIN(min,value) if(min>value) min=value
#define SWAP_MAX(max,value) if(max<value) max=value

#define MAG_ITERATIONS 100

namespace Move
{
	class MoveManager;

	struct MoveCalibrationData
	{
		Mat3 gyroGain;

		Vec3 accBias;
		Mat3 accGain;

		Vec3 magBias;
		Mat3 magGain;
	};

	class MoveCalibration
	{
	private:
		int id;
		char deviceName[25];

		bool calibrated;

		Vec3 magBuf[MAG_ITERATIONS];
		Vec3 magRef[MAG_ITERATIONS];
		Quat lastOri;
		int magPosition;
		float timer;

		MoveCalibrationData data;	
		MoveRawCalibration* rawData;

	public:
		MoveCalibration(int id);
		//MoveCalibration(char btMac[18]);
		~MoveCalibration(void);

		void Update(Vec3 mag, Quat ref, float deltat);
		MoveCalibrationData getCalibrationData();

		bool isCalibrated(){return calibrated;}
		bool initialCalibration();
	private:		
		
		double integrateMagError(std::vector<double> x);
		double integrateAccError(std::vector<double> x);
		double integrateGyroError(std::vector<double> x);
	};

}
