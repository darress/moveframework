#include "MoveManager.h"
#include "MoveDevice.h"
#include "MoveRawCalibration.h"

namespace Move
{
	MoveManager::MoveManager(void)
	{
		FPS=0;
		eyeInt=0;
		moveCount=-1;
	}


	MoveManager::~MoveManager(void)
	{
		for (int i=0;i<moveCount;++i)
		{
			delete moves[i];
		}
		if (eyeInt)
		{
			delete eyeInt;
		}
		MoveDevice::CloseMoves();
	}

	int MoveManager::initMoves()
	{
		//if already initialized or there is no moves found, return -1
		if (moveCount!=-1)
		{
			return moveCount;
		}
		if (!MoveDevice::OpenMoves())
		{
			return 0;
		}
		moveCount = MoveDevice::GetMoveCount();

		for (int i=0;i<moveCount;++i)
		{
			MoveController* ctrl=new MoveController(i, this);
			moves.push_back(ctrl);
		}
		return moveCount;
	}

	void MoveManager::getEyeDimensions(int &x, int &y)
	{
		if (eyeInt)
		{
			eyeInt->getDimensions(x,y);
		}
		else
		{
			x=y=0;
		}
	}

	Quat MoveManager::getOrientation(int id)
	{
		if(id>=moveCount)
			return Move::Quat(0,0,0,0);
		return moves[id]->data.orientation;
	}

	Vec3 MoveManager::getPosition(int id)
	{
		if(id>=moveCount)
			return Move::Vec3(0,0,0);
		Move::Vec3 pos = Move::Vec3(moves[id]->data.position.x,-1.0*moves[id]->data.position.y,moves[id]->data.position.z);
		return pos;
	}

	int MoveManager::getFrameRate()
	{
		return FPS;
	}

	bool MoveManager::initCamera(int numMoves)
	{
		eyeInt=new Eye::EyeInterface(numMoves);
		if (!eyeInt->initCamera())
		{
			delete eyeInt;
			eyeInt=0;
			return false;
		}
		return true;
	}

	PBYTE MoveManager::getEyeBuffer()
	{
		if (eyeInt && eyeInt->img)
		{
			return eyeInt->img->data;
		}
		return 0;
	}

	int MoveManager::getTriggerValue(int id)
	{
		return moves[id]->data.trigger;
	}

	void MoveManager::setRumble(int id, int value)
	{
		moves[id]->setRumble(value);
	}

	Vec3 MoveManager::getAngularVelocity(int id)
	{
		return moves[id]->data.angularVelocity;
	}

	Vec3 MoveManager::getAngularAcceleration(int id)
	{
		return moves[id]->data.angularAcceleration;
	}

	Vec3 MoveManager::getVelocity(int id)
	{
		return moves[id]->data.velocity;
	}	

	Vec3 MoveManager::getAcceleration(int id)
	{
		return moves[id]->data.acceleration;
	}

	bool MoveManager::getButtonState(int id, int buttonId)
	{
		return (moves[id]->data.buttons & buttonId)!=0;
	}

	bool MoveManager::pointingToDisplay(int id)
	{
		return moves[id]->data.isOnDisplay;
	}

	Vec3 MoveManager::displayPosition(int id)
	{
		return moves[id]->data.displayPos;
	}

	int MoveManager::pairMoves()
	{
		return MoveDevice::PairMoves();
	}

	void MoveManager::useMagnetometer(int id, bool value)
	{
		moves[id]->UseMagnetometer(value);
	}

	bool MoveManager::isCalibrated(int id)
	{
		return moves[id]->isCalibrated();
	}

	bool MoveManager::startCalibration(int id)
	{
		return moves[id]->StartCalibration();
	}

	void MoveManager::endCalibration(int id)
	{
		moves[id]->EndCalibration();
		//TODO: event handling
		//eventCalibrationDone();
	}

	Eye::EyeInterface* MoveManager::getEye()
	{
		return eyeInt;
	}

	void MoveManager::subsribeMove(IMoveObserver* observer)
	{
		observers.push_back(observer);
	}
	void MoveManager::unsubsribeMove(IMoveObserver* observer)
	{
		observers.remove(observer);
	}

	void MoveManager::moveUpdated(int moveId, MoveData data)
	{
		std::list<IMoveObserver*>::iterator it;
		for ( it=observers.begin() ; it != observers.end(); it++ )
		{
			(*it)->moveUpdated(moveId, data);
		}
	}
	void MoveManager::moveKeyPressed(int moveId, int keyCode)
	{
		std::list<IMoveObserver*>::iterator it;
		for ( it=observers.begin() ; it != observers.end(); it++ )
		{
			(*it)->moveKeyPressed(moveId, keyCode);
		}
	}
	void MoveManager::moveKeyReleased(int moveId, int keyCode)
	{
		std::list<IMoveObserver*>::iterator it;
		for ( it=observers.begin() ; it != observers.end(); it++ )
		{
			(*it)->moveKeyReleased(moveId, keyCode);
		}
	}
}

