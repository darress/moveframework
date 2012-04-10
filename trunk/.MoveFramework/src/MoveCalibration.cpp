#include "MoveCalibration.h"
#include "simplex.h"
#include "IniFile.h"

using namespace std;

namespace Move
{
	MoveCalibration::MoveCalibration(int moveId)
	{
		this->id=moveId;

		char fileName[]="settings.cfg";

		MoveDevice::TMoveBluetooth bt;
		MoveDevice::ReadMoveBluetoothSettings(id,&bt);
		//device name
		sprintf_s(deviceName,"Move_%s",bt.MoveBtMacString);

		try
		{
			data.gyroGain=IniFile::GetMat3("gyroGain", deviceName, fileName);
			data.accBias=IniFile::GetVec3("accBias", deviceName, fileName);
			data.accGain=IniFile::GetMat3("accGain", deviceName, fileName);
			data.magBias=IniFile::GetVec3("magBias", deviceName, fileName);
			data.magGain=IniFile::GetMat3("magGain", deviceName, fileName);
			calibrated=true;
		}
		catch(MoveConfigFileRecordNotFoundException)
		{
			calibrated=false;
		}
		rawData=0;

		magPosition=-1;
		timer=5.0f;
	}

	MoveCalibration::~MoveCalibration(void)
	{
	}

	void MoveCalibration::Update(Vec3 mag, Quat ref, float deltat)
	{
		if (timer>0.0f)
		{
			timer-=deltat;
			return;
		}
		if (mag.x==0 && mag.y==0 && mag.z==0)
			return;

		if (magPosition==-1)
		{
			lastOri=ref;
			magBuf[++magPosition]=mag;
			Vec3 front=ref.GetColumn2();
			magRef[magPosition]=Vec3(front.x,front.y,front.z);
			return;
		}

		ref.Normalize();

		// calculate relative vector between previous measure point and the current orientation
		if ((ref|lastOri)<0)
			ref=Quat(-ref.w,-1.0f*ref.v);
		Vec3 axis=(!ref*lastOri).v;

		// if there is enough turn between 2 measure points
		if (axis.length()>.3f)
		{
			lastOri=ref;
			magBuf[++magPosition]=mag;
			Vec3 front=ref.GetColumn2();
			magRef[magPosition]=Vec3(front.x,front.x,front.y);
		}

		if (magPosition>=MAG_ITERATIONS-1)
		{
			//initialize the algorithm
			Vec3 min, max, bias, gain;
			min=magBuf[0];
			max=magBuf[0];
			for (int i=1; i<MAG_ITERATIONS; i++)
			{
				SWAP_MIN(min.x,magBuf[i].x);
				SWAP_MIN(min.y,magBuf[i].y);
				SWAP_MIN(min.z,magBuf[i].z);
				SWAP_MAX(max.x,magBuf[i].x);
				SWAP_MAX(max.y,magBuf[i].y);
				SWAP_MAX(max.z,magBuf[i].z);
			}
			bias=(max+min)*0.5f;
			gain=2.0f/(max-min);

			std::vector<double> init;
			std::vector<double> result;

			init.clear();

			init.push_back(gain.x);
			init.push_back(0.00001);
			init.push_back(0.00001);
			init.push_back(0.00001);
			init.push_back(gain.y);
			init.push_back(0.00001);
			init.push_back(0.00001);
			init.push_back(0.00001);
			init.push_back(gain.z);
			init.push_back(bias.x);
			init.push_back(bias.y);
			init.push_back(bias.z);

			result.clear();
			SpecVectorFunctor<double,MoveCalibration> ft(this, &MoveCalibration::integrateMagError);
			result=BT::Simplex(ft, init);

			data.magGain=Mat3(result[0],result[1],result[2],result[3],result[4],result[5],result[6],result[7],result[8]);
			data.magBias=Vec3(result[9],result[10],result[11]);

			magPosition=-1;
			timer=1000;
		}
	}

	MoveCalibrationData MoveCalibration::getCalibrationData()
	{
		return data;
	}

	double MoveCalibration::integrateGyroError(std::vector<double> x)
	{
		double error=0.0;

		Vec3 point;

		Vec3 objective[3];
		objective[0]=Vec3( 2*PI, 0, 0);
		objective[1]=Vec3( 0, 2*PI, 0 );
		objective[2]=Vec3( 0, 0, 2*PI );

		Mat3 gain=Mat3(x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8]);

		for (int i=0; i<3; i++)
		{
			//remove bias
			point=(rawData->GyroVectors[i]-(rawData->GyroBiasVectors[1]*rawData->UnknownVectors[1]));
			//convert from 80 rpm to 1 rotate per second
			point*=0.75f;
			//calculate error vector
			point=point*gain-objective[i];
			//integrate the error
			error+=abs((double)point.length2());
		}

		return error;
	}

	double MoveCalibration::integrateMagError(std::vector<double> x)
	{
		double error=0.0;
		Vec3 point;

		Mat3 gain=Mat3(x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8]);
		Vec3 bias=Vec3(x[9],x[10],x[11]);

		for (int i=1; i<MAG_ITERATIONS; i++)
		{
			//calculate calibrated point
			point=(magBuf[i]-bias)*gain;
			//integrate the error
			error+=(magRef[i]-point).length2();
		}
		return error/(double)MAG_ITERATIONS;
	}

	double MoveCalibration::integrateAccError(std::vector<double> x)
	{
		double error=0.0;

		Vec3 point;

		Vec3 objective[6];

		objective[0]=Vec3( -9.81f, 0, 0 );
		objective[1]=Vec3( 9.81f, 0, 0 );
		objective[2]=Vec3( 0, -9.81f, 0 );
		objective[3]=Vec3( 0, 9.81f, 0 );
		objective[4]=Vec3( 0, 0, -9.81f );
		objective[5]=Vec3( 0, 0, 9.81f);

		Mat3 gain=Mat3(x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8]);
		Vec3 bias=Vec3(x[9],x[10],x[11]);

		for (int i=0; i<6; i++)
		{
			//calculate error vector
			point=(rawData->AccVectors[i]-bias)*gain-objective[i];
			//integrate the error
			error+=abs((double)point.length2());
		}
		return error;
	}

	//move calibration
	bool MoveCalibration::initialCalibration()
	{
		MoveDevice::TMoveCalib calib;
		if (!MoveDevice::ReadMoveCalibration(id,&calib))
			return false;
		rawData=new MoveRawCalibration(calib);

		std::vector<double> init;
		std::vector<double> result;

		data.magBias=Vec3::ZERO;
		data.magGain=Mat3();


		//ACCELEROMETER
		init.clear();
		init.push_back(0.0022);
		init.push_back(0.0001);
		init.push_back(0.0001);
		init.push_back(0.0001);
		init.push_back(0.0022);
		init.push_back(0.0001);
		init.push_back(0.0001);
		init.push_back(0.0001);
		init.push_back(0.0022);
		init.push_back(0.0001);
		init.push_back(0.0001);
		init.push_back(0.0001);


		result.clear();
		SpecVectorFunctor<double,MoveCalibration> ft2(this, &MoveCalibration::integrateAccError);
		result=BT::Simplex(ft2, init);

		data.accGain=Mat3(result[0],result[1],result[2],result[3],result[4],result[5],result[6],result[7],result[8]);
		data.accBias=Vec3(result[9],result[10],result[11]);


		//GYROSCOPE
		init.clear();
		init.push_back(0.0016);
		init.push_back(0.0001);
		init.push_back(0.0001);
		init.push_back(0.0001);
		init.push_back(0.0016);
		init.push_back(0.0001);
		init.push_back(0.0001);
		init.push_back(0.0001);
		init.push_back(0.0016);

		result.clear();
		SpecVectorFunctor<double,MoveCalibration> ft3(this, &MoveCalibration::integrateGyroError);
		result=BT::Simplex(ft3, init);

		data.gyroGain=Mat3(result[0],result[1],result[2],result[3],result[4],result[5],result[6],result[7],result[8]);
		
		////save calibration
		IniFile::SetValue("gyroGain", data.gyroGain, deviceName, "settings.cfg");
		IniFile::SetValue("accBias", data.accBias, deviceName, "settings.cfg");
		IniFile::SetValue("accGain", data.accGain, deviceName, "settings.cfg");
		IniFile::SetValue("magBias", data.magBias, deviceName, "settings.cfg");
		IniFile::SetValue("magGain", data.magGain, deviceName, "settings.cfg");
		
		delete rawData;
		calibrated=true;
	}
}