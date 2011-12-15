#include "MovePrecompiled.h"
#include "MoveController.h"

namespace Move
{
	MoveController::MoveController(int id, MoveManager* manager)
	{
		this->id=id;
		this->manager=manager;

		calibration = new MoveCalibration(id, manager);
		orientation = new MoveOrientation(calibration);

		lastSeqNumber=-1;
		firstPackage=true;

		packageGot=0;
		missingPackage=0;

		estimatedVelocity=Vector3(0,0,0);
		estimatedPosition=Vector3(0,0,0);
	}


	MoveController::~MoveController(void)
	{
	}

	void MoveController::Update()
	{
		while (true)
		{
			MoveDevice::TMove m, old;
			if (!ReadMove(id, &m, &old)) continue;

			if (firstPackage)
			{
				lastSeqNumber = m.SeqNumber;
				lastTimestamp = m.Timestamp;
				data.buttons = m.Buttons;
				firstPackage=false;
				continue;
			}

			if (m.SeqNumber == lastSeqNumber)
				continue;
			if (lastSeqNumber!=m.SeqNumber-1 && lastSeqNumber!=m.SeqNumber+15 && packageGot>200)
				missingPackage++;
			packageGot++;
			int ticksEllapsed =m.Timestamp-lastTimestamp;
			if (ticksEllapsed<0)
				ticksEllapsed+=65536;
			float timeEllapsed=(float)ticksEllapsed/100000.0f;
			lastSeqNumber = m.SeqNumber;
			lastTimestamp = m.Timestamp;

			Vector3 MoveAcc, MoveGyro, MoveMag;
			MoveAcc = Vector3(old.RawForceX,old.RawForceY,old.RawForceZ);
			MoveGyro = Vector3(-old.RawGyroPitch,old.RawGyroYaw,-old.RawGyroRoll);
			MoveMag = Vector3(old.RawMagnetX,old.RawMagnetY,old.RawMagnetZ);

			calibration->Update(MoveAcc,MoveGyro,MoveMag,timeEllapsed/2.0f);
			orientation->Update(MoveAcc,MoveGyro,MoveMag,timeEllapsed/2.0f);

			MoveAcc = Vector3(m.RawForceX,m.RawForceY,m.RawForceZ);
			MoveGyro = Vector3(-m.RawGyroPitch,m.RawGyroYaw,-m.RawGyroRoll);
			MoveMag = Vector3(m.RawMagnetX,m.RawMagnetY,m.RawMagnetZ);

			calibration->Update(MoveAcc,MoveGyro,MoveMag,timeEllapsed/2.0f);

			if (calibration->isCalibrated())
			{
				orientation->Update(MoveAcc,MoveGyro,MoveMag,timeEllapsed/2.0f);

				data.orientation=orientation->GetOrientation();
				data.angularVelocity=orientation->GetAngularVelocity();
				data.angularAcceleration=orientation->GetAngularAcceleration();
			}

			if (m.Buttons & MoveDevice::B_SQUARE && !(data.buttons & MoveDevice::B_SQUARE))
			{
				orientation->HighGains();
			}
			if (m.Buttons & MoveDevice::B_CROSS && !(data.buttons & MoveDevice::B_CROSS))
			{
				orientation->Reset();
			}
			if ((m.Buttons & MoveDevice::B_CIRCLE) && !(data.buttons & MoveDevice::B_CIRCLE))
			{
				estimatedVelocity=Vector3(0,0,0);
				estimatedPosition=Vector3(0,0,0);
			}
			if (m.Buttons & MoveDevice::B_TRIANGLE)
			{

			}
			if (m.Buttons & MoveDevice::B_PS)
			{
				//device->closeDevice();
			}
			for (int i=0; i<24; i++)
			{
				int key=(int)pow(2.0,i);
				if ((m.Buttons & key) && !(data.buttons & key))
				{
					manager->moveKeyPressed(id, key);
				}
				if (!(m.Buttons & key) && (data.buttons & key))
				{
					manager->moveKeyReleased(id, key);
				}
			}
			data.buttons = m.Buttons;

			////estimated acceleration from accelerometer
			//Vector3 estimatedAcceleration = orientation->GetEstimatedAcceleration();
			//estimatedVelocity += estimatedAcceleration * timeEllapsed;
			//estimatedPosition += estimatedVelocity* timeEllapsed;
			//data.position=estimatedPosition*20.0f;

			//if camera is capturing
			if (manager->getEye())
			{
				//TODO: initial solution
				data.isOnDisplay=false;
				if (manager->getEye()->balls[id].ballFoundOut)
				{
					Vector3 previousPosition=data.position;
					Vector3 previousVelocity=data.velocity;
					data.position.x=manager->getEye()->balls[id].ballX;
					data.position.y=manager->getEye()->balls[id].ballY;
					data.position.z=manager->getEye()->balls[id].ballZ;
					data.velocity=(data.position-previousPosition)/timeEllapsed;
					data.acceleration=(data.velocity-previousVelocity)/timeEllapsed;

					//TODO: initial solution
					if (data.position.x>-50.0f && data.position.x<50.0f && data.position.y>-50.0f && data.position.y<50.0f)
					{
						data.isOnDisplay=true;
						data.displayPos=(data.position+Vector3(50.0f,50.0f,0.0f))/100.0f;
					}
				}
				else
					data.position=Vector3(0,0,0);

				MoveDevice::SetMoveColour(id,manager->getEye()->balls[id].ballOutColor.r,manager->getEye()->balls[id].ballOutColor.g,manager->getEye()->balls[id].ballOutColor.b);
			}
			manager->moveUpdated(id, data);
		}
	}

	bool MoveController::isCalibrated()
	{
		return calibration->isCalibrated();
	}

	bool MoveController::StartCalibration()
	{
		return calibration->startCalibration();
	}

	void MoveController::EndCalibration()
	{
		calibration->endCalibration();
	}

}