#pragma once
#include "MovePrecompiled.h"
#include "MoveRawCalibration.h"
#include "MoveManager.h"

#define SWAP_MIN(min,value) if(min>value) min=value
#define SWAP_MAX(max,value) if(max<value) max=value
#define SKIPDATA 2
#define MAG_ITERATIONS 100

namespace Move
{
	class MoveManager;

	struct MoveCalibrationData
	{
		Matrix3 gyroGain;

		Vector3 accBias;
		Matrix3 accGain;

		Vector3 magBias;
		Vector3 magGain;
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
		Vector3* magBuf;
		int skipData;

		MoveCalibrationData data;	
		MoveRawCalibration* rawData;

	public:
		MoveCalibration(int id, MoveManager* manager);
		//MoveCalibration(char btMac[18]);
		~MoveCalibration(void);

		void Update(Vector3 acc, Vector3 gyro, Vector3 mag, float deltat);
		MoveCalibrationData getCalibrationData();

		//move calibration
		bool startCalibration();
		void endCalibration();
		bool isCalibrated(){return calibrated;}

	public:
		static MoveCalibration* instance;
		static double integrateMagError(std::vector<double> x);
		static double integrateAccError(std::vector<double> x);
		static double integrateGyroError(std::vector<double> x);
	};

}
