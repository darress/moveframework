#include "NavController.h"
#include "MoveDevice.h"
#include "MoveManager.h"

namespace Move
{

	NavController::NavController(int navId)
		: navId(navId)
	{
		_hThread = CreateThread(NULL, 0, &NavController::controlThread, this, 0, 0);
	}


	NavController::~NavController()
	{
	}

	DWORD WINAPI NavController::controlThread(LPVOID instance)
	{
		NavController *pThis = (NavController*)instance;
		pThis->Update();
		return 0;
	}

	void NavController::Update()
	{
		MoveManager* manager=MoveManager::getInstance();
		while (true)
		{
			MoveDevice::TNav m;
			if (!ReadNav(navId, &m))
				continue;

			//TODO: LOCK
			for (int i=0; i<20; i++)
			{
				int key=0x1<<i;
				if ((m.Buttons & key) && !(data.buttons & key))
				{
					manager->navKeyPressed(navId, (MoveButton)key);
				}
				if (!(m.Buttons & key) && (data.buttons & key))
				{
					manager->navKeyReleased(navId, (MoveButton)key);
				}
			}

			data.buttons=m.Buttons;
			data.stickX=m.LeftStickX;
			data.stickY=m.LeftStickY;
			data.trigger1=m.L2Analog;
			data.trigger2=m.L1Analog;
			manager->navUpdated(navId, data);
		}
	}

	NavData NavController::getNavData()
	{
		//TODO: LOCK
		return data;
	}

}