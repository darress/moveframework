#pragma once

#include "MoveRawCalibration.h"
#include "MoveManager.h"
#include "Mat3.h"
#include "MoveIncludes.h"

#define SWAP_MIN(min,value) if(min>value) min=value
#define SWAP_MAX(max,value) if(max<value) max=value
#define SKIPDATA 2
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
		Vec3 magGain;
	};

	class MoveCalibration
	{
	private:
		int id;
		char deviceName[25];
		MoveManager* manager;

		bool calibrated;
		bool isCalibrating;

		int bufLength;
		Vec3* magBuf;
		int skipData;

		MoveCalibrationData data;	
		MoveRawCalibration* rawData;

	public:
		MoveCalibration(int id, MoveManager* manager);
		//MoveCalibration(char btMac[18]);
		~MoveCalibration(void);

		void Update(Vec3 acc, Vec3 gyro, Vec3 mag, float deltat);
		MoveCalibrationData getCalibrationData();

		//move calibration
		bool startCalibration();
		void endCalibration();
		bool isCalibrated(){return calibrated;}

	private:
		float integrateMagError(std::vector<float> x);
		float integrateAccError(std::vector<float> x);
		float integrateGyroError(std::vector<float> x);
	};

}
