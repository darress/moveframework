#include "MoveController.h"
#include "MoveButton.h"
#include "MoveLock.h"


namespace Move
{
	float MoveController::offsetX, MoveController::offsetY, MoveController::offsetZ;
	MoveController::MoveController(int moveId)
		:id(moveId)
	{
		orientation = new MoveOrientation(id);

		lastSeqNumber=-1;
		firstPackage=true;
		resetTimer=0;

		_hThread = CreateThread(NULL, 0, &MoveController::controlThread, this, 0, 0);
		SetPriorityClass(_hThread,REALTIME_PRIORITY_CLASS);
		SetThreadPriority(_hThread,THREAD_PRIORITY_TIME_CRITICAL);
	}

	DWORD WINAPI MoveController::controlThread(LPVOID instance)
	{
		MoveController *pThis = (MoveController*)instance;
		pThis->Update();
		return 0;
	}

	MoveController::~MoveController(void)
	{
		TerminateThread(_hThread, 0);
		delete orientation;
	}

	MoveData MoveController::getMoveData()
	{
		MoveLock lock(id);
		return MoveManager::getInstance()->getMoveDataEx(id);
	}

	void MoveController::Update()
	{
		MoveManager* manager=MoveManager::getInstance();
		while (true)
		{
			MoveDevice::TMove m, old;
			if (!ReadMove(id, &m, &old)) continue;

			if (firstPackage)
			{
				lastSeqNumber = m.SeqNumber;
				lastTimestamp = m.Timestamp;
				MoveLock flock(id);
				manager->getMoveDataEx(id).buttons = m.Buttons;
				flock.release();
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

			Vec3 MoveAcc, MoveGyro, MoveMag;
			MoveAcc = Vec3(old.RawForceX,old.RawForceY,old.RawForceZ);
			MoveGyro = Vec3(-old.RawGyroPitch,old.RawGyroYaw,-old.RawGyroRoll);
			MoveMag = Vec3(0,0,0);
			orientation->Update(MoveAcc,MoveGyro,MoveMag,timeEllapsed/2.0f);

			MoveAcc = Vec3(m.RawForceX,m.RawForceY,m.RawForceZ);
			MoveGyro = Vec3(-m.RawGyroPitch,m.RawGyroYaw,-m.RawGyroRoll);
			MoveMag = Vec3(m.RawMagnetX,m.RawMagnetY,m.RawMagnetZ);
			orientation->Update(MoveAcc,MoveGyro,MoveMag,timeEllapsed/2.0f);

			MoveLock lock(id);
			manager->getMoveDataEx(id).orientation=orientation->GetOrientation();
			manager->getMoveDataEx(id).angularVelocity=orientation->GetAngularVelocity();
			manager->getMoveDataEx(id).angularAcceleration=orientation->GetAngularAcceleration();
			for (int i=0; i<17; i++)
			{
				int key=0x10<<i;
				if ((m.Buttons & key) && !(manager->getMoveDataEx(id).buttons & key))
				{
					manager->moveKeyPressed(id, (MoveButton)key);
				}
				if (!(m.Buttons & key) && (manager->getMoveDataEx(id).buttons & key))
				{
					manager->moveKeyReleased(id, (MoveButton)key);
				}
			}
			manager->getMoveDataEx(id).buttons = m.Buttons;
			manager->getMoveDataEx(id).trigger = m.TAnalog;

			manager->moveUpdated(id);

			lock.release();

			//handle resets
			if (m.Buttons & Move::B_PS)
			{
				resetTimer+=timeEllapsed;
				orientation->Reset();
				if (resetTimer>2.0f && manager->getEye())
				{
					manager->getEye()->resetPosition(id);
					resetTimer=0;
				}
			}
			else
			{
				resetTimer=0;
			}

			
		}
	}

	void MoveController::useMagnetometers(bool value)
	{
		orientation->UseMagnetometer(value);
	}

	void MoveController::setRumble(int value)
	{
		MoveDevice::SetMoveRumble(id, value);
	}

	void MoveController::setOrientationGain(float gain)
	{
		orientation->setOrientationGain(gain);
	}

	void MoveController::calibrateMagnetometer()
	{
		orientation->calibrateMagnetometer();
	}
}