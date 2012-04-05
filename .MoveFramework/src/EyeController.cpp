
#include "EyeController.h"
#include "MoveLock.h"

namespace Move
{

	EyeController::EyeController(MoveManager* manager)
	{
		this->manager=manager;
		img=0;
		_cam=0;
		ballManager=0;
	}

	EyeController::~EyeController(void)
	{
		TerminateThread(_hThread,0);

		if (img)
			delete(img);

		if (_cam)
		{
			CLEyeCameraStop(_cam);
			CLEyeDestroyCamera(_cam);
		}

		if (ballManager)
			delete ballManager;
	}

	bool EyeController::initCamera(int numMoves)
	{
		this->numMoves=numMoves;

		GUID _cameraGUID;

		_cameraGUID = CLEyeGetCameraUUID(0);
		if (_cameraGUID==GUID_NULL)
			return false;
		_cam = CLEyeCreateCamera(_cameraGUID, CLEYE_COLOR_PROCESSED, CLEYE_VGA, 75);
		if (_cam==0)
			return false;
		CLEyeCameraGetFrameDimensions(_cam, width, height);

		CLEyeSetCameraParameter(_cam, CLEYE_GAIN, 0);
		CLEyeSetCameraParameter(_cam, CLEYE_AUTO_EXPOSURE, true);
		CLEyeSetCameraParameter(_cam, CLEYE_AUTO_WHITEBALANCE, true);
		//CLEyeSetCameraParameter(_cam, CLEYE_WHITEBALANCE_RED, 255);
		//CLEyeSetCameraParameter(_cam, CLEYE_WHITEBALANCE_GREEN, 255);
		//CLEyeSetCameraParameter(_cam, CLEYE_WHITEBALANCE_BLUE, 255);
		CLEyeSetCameraParameter(_cam, CLEYE_HFLIP, true);
		//CLEyeSetCameraParameter(_cam, CLEYE_VFLIP, true);

		_hThread = CreateThread(NULL, 0, &EyeController::CaptureThread, this, 0, 0);
		SetPriorityClass(_hThread,REALTIME_PRIORITY_CLASS);
		SetThreadPriority(_hThread,THREAD_PRIORITY_HIGHEST);
		return true;
	}

	void EyeController::Run()
	{
		CLEyeCameraStart(_cam);
		Sleep(100);

		img = new EyeImage(width,height);

		ballManager= new BallManager(numMoves, img);

		while(true)
		{
			bool ret = CLEyeCameraGetFrame(_cam, img->data);

			std::vector<Vec3> positions = ballManager->findBalls();
			
			for (int i=0; i<numMoves; i++)
			{
				MoveLock lock(i);
				manager->getMoveDataEx(i).position=positions[i];
			}
		}
	}

	DWORD WINAPI EyeController::CaptureThread(LPVOID instance)
	{
		EyeController *pThis = (EyeController *)instance;
		pThis->Run();
		return 0;
	}


	unsigned char* EyeController::getEyeBuffer(void)
	{
		if (!img)
			return 0;
		return img->data;
	}

	unsigned char* EyeController::getMaskBuffer(int moveId)
	{
		if (!ballManager)
			return 0;
		return ballManager->getMaskBuffer(moveId);
	}

	void EyeController::getEyeDimensions(int &x, int &y)
	{
		x=width; y=height;
	}

	void EyeController::useAutomaticColors(bool use)
	{
	
	}

	void EyeController::setColor(int moveId, int r, int g, int b)
	{
	
	}

	void EyeController::resetPosition(int moveId)
	{

	}

}

