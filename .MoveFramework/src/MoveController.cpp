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
			int ticksEllapsed =m.Timestamp-lastTimestamp;
			if (ticksEllapsed<0)
				ticksEllapsed+=65536;
			float timeEllapsed=(float)ticksEllapsed/100000.0f;
			lastSeqNumber = m.SeqNumber;
			lastTimestamp = m.Timestamp;

			Vector3 MoveAcc, MoveGyro, MoveMag;
			MoveAcc = Vector3(old.RawForceX,old.RawForceY,old.RawForceZ);
			MoveGyro = Vector3(-old.RawGyroPitch,old.RawGyroYaw,-old.RawGyroRoll);
			MoveMag = Vector3(0,0,0);

			calibration->Update(MoveAcc,MoveGyro,MoveMag,timeEllapsed/2.0f);

			if (calibration->isCalibrated())
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
			for (int i=0; i<17; i++)
			{
				int key=0x10<<i;
				if ((m.Buttons & key) && !(data.buttons & key))
				{
					manager->moveKeyPressed(id, key);
					if (key==MoveDevice::B_PS)
						orientation->Reset();
				}
				if (!(m.Buttons & key) && (data.buttons & key))
				{
					manager->moveKeyReleased(id, key);
				}
			}
			data.buttons = m.Buttons;
			data.trigger = m.TAnalog;

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

	void MoveController::UseMagnetometer(bool value)
	{
		orientation->UseMagnetometer(value);
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

	void MoveController::setRumble(int value)
	{
		MoveDevice::SetMoveRumble(id, value);
	}
}