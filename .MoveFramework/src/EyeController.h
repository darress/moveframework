#pragma once

#include "kalman.h"
#include "EyeImage.h"
#include "CLEyeMulticam.h"
#include "IEyeController.h"
#include "BallManager.h"
#include "MoveManager.h"

namespace Move
{
	class MoveManager;

	class EyeController : public IEyeController
	{
		HANDLE _hThread;
		CLEyeCameraInstance _cam;

		int width, height;
		int numMoves;

		BallManager* ballManager;

	public:

		EyeImage *img;
		

	public:
		EyeController();
		~EyeController();

		bool initCamera(int numMoves);

		unsigned char* getEyeBuffer();
		unsigned char* getMaskBuffer(int moveId);
		void getEyeDimensions(int &x, int &y);

		void useAutomaticColors(bool use);
		void setColor(int moveId, int r, int g, int b);
		void resetPosition(int moveId);

	private:
		void Run();
		static DWORD WINAPI CaptureThread(LPVOID instance);
	};
}
