#include "MovePrecompiled.h"
#include "MoveCalibration.h"
#include "simplex.h"
#include "IniFile.h"

using namespace std;

namespace Move
{
	MoveCalibration* MoveCalibration::instance = 0;

	MoveCalibration::MoveCalibration(int id, MoveManager* manager)
	{
		this->id=id;
		isCalibrating=false;
		this->manager=manager;

		char fileName[]="settings.cfg";

		MoveDevice::TMoveBluetooth bt;
		MoveDevice::ReadMoveBluetoothSettings(id,&bt);
		//device name
		sprintf_s(deviceName,"Move_%s",bt.MoveBtMacString);

		try
		{
			data.gyroGain=CIniFile::GetMatrix3("gyroGain", deviceName, fileName);
			data.accBias=CIniFile::GetVector3("accBias", deviceName, fileName);
			data.accGain=CIniFile::GetMatrix3("accGain", deviceName, fileName);
			data.magBias=CIniFile::GetVector3("magBias", deviceName, fileName);
			data.magGain=CIniFile::GetVector3("magGain", deviceName, fileName);
			calibrated=true;
		}
		catch(MoveConfigFileRecordNotFoundException)
		{
			calibrated=false;
		}
		magBuf=0;
		rawData=0;
	}

	MoveCalibration::~MoveCalibration(void)
	{
	}

	void MoveCalibration::Update(Vector3 acc, Vector3 gyro, Vector3 mag, float deltat)
	{
		// if there is no phase running
		if (!isCalibrating) return;
		
		if (bufLength<2000 && (skipData--)<=0)
		{
			magBuf[bufLength]=mag;
			bufLength++;
			skipData=SKIPDATA;
		}
	}

	MoveCalibrationData MoveCalibration::getCalibrationData()
	{
		return data;
	}

	//move calibration
	bool MoveCalibration::startCalibration()
	{
		MoveDevice::TMoveCalib calib;
		if (!MoveDevice::ReadMoveCalibration(id,&calib))
			return false;
		rawData=new MoveRawCalibration(calib);

		isCalibrating=true;

		magBuf=new Vector3[2000];

		bufLength=0;

		return true;
	}

	double MoveCalibration::integrateGyroError(std::vector<double> x)
	{
		double error=0.0;
		MoveRawCalibration* rawData=MoveCalibration::instance->rawData;

		Vector3 point;

		Vector3 objective[3];
		objective[0]=Vector3( 2*PI, 0, 0);
		objective[1]=Vector3( 0, 2*PI, 0 );
		objective[2]=Vector3( 0, 0, 2*PI );

		Matrix3 gain=Matrix3(x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8]);

		for (int i=0; i<3; i++)
		{
			//remove bias
			point=(rawData->GyroVectors[i]-(rawData->GyroBiasVectors[1]*rawData->UnknownVectors[1]));
			//convert from 80 rpm to 1 rotate per second
			point*=0.75;
			//calculate error vector
			point=gain*point-objective[i];
			//integrate the error
			error+=fabs((double)glm::length2(point));
		}

		return error;
	}

	double MoveCalibration::integrateMagError(std::vector<double> x)
	{
		double error=0.0f;
		Vector3 point;

		Vector3 bias=Vector3(x[0],x[1],x[2]);
		Vector3 gain=Vector3(x[3],x[4],x[5]);

		for (int i=1; i<MoveCalibration::instance->bufLength; i++)
		{
			//calculate calibrated point
			point=(MoveCalibration::instance->magBuf[i]-bias)*gain;
			//integrate the error
			error+=fabs(1.0-(double)glm::length2(point));
		}
		return error/(double)MoveCalibration::instance->bufLength;
	}

	double MoveCalibration::integrateAccError(std::vector<double> x)
	{
		double error=0.0f;
		MoveRawCalibration* rawData=MoveCalibration::instance->rawData;

		Vector3 point;

		Vector3 objective[6];

		objective[0]=Vector3( -9.81, 0, 0 );
		objective[1]=Vector3( 9.81, 0, 0 );
		objective[2]=Vector3( 0, -9.81, 0 );
		objective[3]=Vector3( 0, 9.81, 0 );
		objective[4]=Vector3( 0, 0, -9.81 );
		objective[5]=Vector3( 0, 0, 9.81);

		Matrix3 gain=Matrix3(x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8]);
		Vector3 bias=Vector3(x[9],x[10],x[11]);

		for (int i=0; i<6; i++)
		{
			//calculate error vector
			point=gain*(rawData->AccVectors[i]-bias)-objective[i];
			//integrate the error
			error+=fabs((double)glm::length2(point));
		}
		return error;
	}


	void MoveCalibration::endCalibration()
	{
		isCalibrating=false;

		MoveCalibration::instance=this;

		std::vector<double> init;
		std::vector<double> result;

		double error1, error2;

		//MAGNETOMETER
		//initialize the algorithm
		Vector3 min, max, bias, gain;
		min=magBuf[0];
		max=magBuf[0];
		for (int i=1; i<bufLength; i++)
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

		init.clear();
		init.push_back(bias.x);
		init.push_back(bias.y);
		init.push_back(bias.z);
		init.push_back(gain.x);
		init.push_back(gain.y);
		init.push_back(gain.z);


		result.clear();
		result=BT::Simplex(&Move::MoveCalibration::integrateMagError, init);

		data.magBias=Vector3(result[0],result[1],result[2]);
		data.magGain=Vector3(result[3],result[4],result[5]);


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


		error1=integrateAccError(init);
		result.clear();
		result=BT::Simplex(&Move::MoveCalibration::integrateAccError, init);
		error2=integrateAccError(result);

		data.accGain=Matrix3(result[0],result[1],result[2],result[3],result[4],result[5],result[6],result[7],result[8]);
		data.accBias=Vector3(result[9],result[10],result[11]);


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

		error1=integrateGyroError(init);
		result.clear();
		result=BT::Simplex(&Move::MoveCalibration::integrateGyroError, init);
		error2=integrateGyroError(result);
		data.gyroGain=Matrix3(result[0],result[1],result[2],result[3],result[4],result[5],result[6],result[7],result[8]);



		////save calibration
		CIniFile::SetValue("gyroGain", data.gyroGain, deviceName, "settings.cfg");
		CIniFile::SetValue("accBias", data.accBias, deviceName, "settings.cfg");
		CIniFile::SetValue("accGain", data.accGain, deviceName, "settings.cfg");
		CIniFile::SetValue("magBias", data.magBias, deviceName, "settings.cfg");
		CIniFile::SetValue("magGain", data.magGain, deviceName, "settings.cfg");
		
		delete[] magBuf;
		delete rawData;
		calibrated=true;
		manager->calibrationDone(id);
	}
}