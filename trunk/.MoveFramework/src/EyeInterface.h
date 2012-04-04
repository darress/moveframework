#pragma once

#include "kalman.h"
#include "EyeImage.h"
#include "CLEyeMulticam.h"

namespace Eye
{
	class EyeInterface
	{
		HANDLE _hThread;
		PBYTE pCapBuffer;
		CLEyeCameraInstance _cam;
		Move::Kalman kballz[2];

		int width, height;
		int numMoves;

	public:
		EyeImage *img;

	public:
		Ball balls[2];

	public:
		EyeInterface(int numMoves);
		bool initCamera();
		~EyeInterface(void);

		void getDimensions(int &x, int &y)
		{
			x=width; y=height;
		}

	private:
		void Run();
		static DWORD WINAPI CaptureThread(LPVOID instance);
		
	};
}
