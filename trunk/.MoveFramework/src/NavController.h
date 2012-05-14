#pragma once

#include "INavController.h"
#include "MoveIncludes.h"

namespace Move
{

	class NavController : public INavController
	{
		NavData data;
		int navId;

		HANDLE _hThread;

	public:
		NavController(int navId);
		~NavController();

		NavData getNavData();
		

	private:
		static DWORD WINAPI controlThread(LPVOID instance);
		void Update();
	};

}