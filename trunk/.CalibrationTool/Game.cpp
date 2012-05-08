#include "stdafx.h"
#include "Game.h"
#include "MoveButton.h"
#include "MoveData.h"
#include <iostream>
#include <iomanip>

Game::Game()
{
	guiInitialized=false;
	cameraWorks=false;
	automaticColors=true;
	r=g=b=0;
	cameraControl=false;
	showMask=-1;
}

Game::~Game()
{
}

bool Game::configure()
{
	bool ret = BaseApplication::configure();
	
	int height = mWindow->getHeight();
	int width = mWindow->getWidth();

	return ret;
}

void Game::createScene()
{
	MyGUI::LayoutManager::getInstance().loadLayout("main.layout");
	initMove();

	for (int i=0; i<numMoves; i++)
	{
		// a demo to show how skeletal animation works if joints must be moved by piece
		char tmp[20];
		sprintf(tmp,"move%d",i);
		Ogre::Entity* moveEntity = mSceneMgr->createEntity(tmp, "move.mesh");
		moveNode[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		moveNode[i]->attachObject(moveEntity);
		moveNode[i]->scale(0.8f,0.8f,0.8f);
	}

    // Set ambient light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

    // Create a light
    Ogre::Light* l = mSceneMgr->createLight("MainLight");
    l->setPosition(20,80,50);

	initGui();
	guiInitialized=true;
}

void Game::initGui()
{
	if (cameraWorks)
	{
		int eyeX, eyeY;
		move->getEye()->getEyeDimensions(eyeX,eyeY);

		//create texture for camera image
		camImage = Ogre::TextureManager::getSingleton().createManual(
			"CameraImage", // name
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,      // type
			eyeX, eyeY,         // width & height
			0,                // number of mipmaps
			Ogre::PF_X8R8G8B8,     // pixel format
			Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

		//create texture for camera image
		maskImage = Ogre::TextureManager::getSingleton().createManual(
			"MaskImage", // name
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,      // type
			eyeX, eyeY,         // width & height
			0,                // number of mipmaps
			Ogre::PF_L16,     // pixel format
			Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

		//set texture of eye image
		mGUI->findWidget<MyGUI::ImageBox>("Eye Image")->setImageTexture("CameraImage");

		//set texture of mask image
		mGUI->findWidget<MyGUI::ImageBox>("Mask Image")->setImageTexture("MaskImage");
		mGUI->findWidget<MyGUI::ImageBox>("Mask Image")->setVisible(false);
	}
	else
	{
		mGUI->findWidget<MyGUI::TextBox>("CameraInfo")->setCaption("No PS Eye found.");
	}
	mGUI->findWidget<MyGUI::TextBox>("PairInfo")->setCaption("To pair devices connected with USB, press P on your keyboard.\nTo calibrate the magnetometer of a device, press START on that device.");
	if (numMoves<1)
	{
		mGUI->findWidget<MyGUI::TextBox>("Info")->setCaption("No device is connected via Bluetooth. Nothing to calibrate.");
		mGUI->findWidget<MyGUI::TextBox>("Moves")->setCaption("No device is found.");
	}
}

void Game::guiExitClick(MyGUI::Widget* _widget)
{
	mShutDown=true;
}

bool Game::keyPressed( const OIS::KeyEvent &arg )
{
	if (arg.key == OIS::KC_P)
    {

		int num = move->pairMoves();
		if (num==0)
		{
			mGUI->findWidget<MyGUI::TextBox>("PairInfo")->setCaption("No new device has been paired, to try again, press P.");
		}
		else if (num==-1)
		{
			mGUI->findWidget<MyGUI::TextBox>("PairInfo")->setCaption("No Bluetooth dongle found, please connect one!");
		}
		else
		{
			char tmp[100];
			sprintf(tmp,"%d new devices has been paired, to pair again, press P.",num);
			mGUI->findWidget<MyGUI::TextBox>("PairInfo")->setCaption(tmp);
		}
	}
	else if (arg.key==OIS::KC_M)
	{
		bool use=true;
		for (int i=0; i<numMoves; i++)
		{
			use=use && !useMagnetometers[i];
		}
		for (int i=0; i<numMoves; i++)
		{
			move->getMove(i)->useMagnetometers(use);
		}
	}
	else if (arg.key==OIS::KC_A)
	{
		automaticColors=!automaticColors;
		move->getEye()->useAutomaticColors(automaticColors);
	}
	else if (arg.key==OIS::KC_C)
	{
		cameraControl=!cameraControl;
	}
	else if (arg.key==OIS::KC_ESCAPE)
	{
		move->unsubsribe(this);
		guiInitialized=false;
	}
	else if (arg.key==OIS::KC_0)
	{
		showMask=-1;
		mGUI->findWidget<MyGUI::ImageBox>("Mask Image")->setVisible(false);
	}
	else if (arg.key==OIS::KC_1 || arg.key==OIS::KC_2)
	{
		showMask=int(arg.key)-2;
		if (showMask>=numMoves)
		{
			showMask=-1;
			mGUI->findWidget<MyGUI::ImageBox>("Mask Image")->setVisible(false);
		}
		else
		{
			mGUI->findWidget<MyGUI::ImageBox>("Mask Image")->setVisible(true);
		}
	}
	return BaseApplication::keyPressed(arg);
}

void Game::moveKeyPressed(int moveId, Move::MoveButton button)
{
	if (button==Move::B_START)
	{
		move->getMove(moveId)->calibrateMagnetometer();
	}
}

void Game::moveKeyReleased(int moveId, Move::MoveButton button)
{

}

void Game::moveNotify(int moveId, Move::MoveMessage message)
{
	if (message==Move::M_UseMagnetometers)
	{
		useMagnetometers[moveId]=true;
		return;
	}
	if (message==Move::M_DoesntUseMagnetometers)
	{
		useMagnetometers[moveId]=false;
		return;
	}

	std::stringstream ss;
	ss << "Device " << moveId << ": ";
	ss.str();
	if (message==Move::M_Calibrated)
	{
		magnetometerCalibrated[moveId]=true;
		ss << "Magnetometers calibrated!";
	}
	if (message==Move::M_CalibrationProcessing)
		ss << "Processing calibration data, please hold the device still!";
	if (message==Move::M_InitialCalibratingDone)
	{
		magnetometerCalibrated[moveId]=false;
		ss << "Acc, Gyro calibrated!";
	}
	if (message==Move::M_RotateMove)
		ss << "Please rotate your device in every possible direction!";


	mGUI->findWidget<MyGUI::TextBox>("Info")->setCaption(ss.str());
}

void Game::moveUpdated(int moveId, Move::MoveData data)
{
	if (!guiInitialized)
		return;
	std::stringstream ss;
	ss.precision(2);
	ss.setf(std::ios_base::fixed);
	ss << numMoves << " devices connected.\n";
	for (int i=0; i<numMoves; i++)
	{
		ss << std::endl << std::endl;

		Move::Vec3 pos = data.position;
		ss <<  "Position: " << pos.x << " " << pos.y << " " << pos.z << std::endl;

		Move::Quat quat = data.orientation;
		ss << "Orient.: " << quat.w << " " << quat.v.x << " " << quat.v.y << " " << quat.v.z << std::endl;

		if (!magnetometerCalibrated[i])
			ss << "The device' magnetometer is not calibrated.\n   Press START to calibrate it.";
		else if (useMagnetometers[i])
			ss << "The device uses magnetometers.\n   Press M to turn it off.\n";
		else
			ss << "The device doesnt use magnetometers.\n   Press M to turn it on.\n";
	}
	//mGUI->findWidget<MyGUI::TextBox>("Moves")->setCaption(ss.str().c_str());
	
	if (!automaticColors)
	{
		if (data.isButtonPressed(Move::B_CIRCLE))
			r=data.trigger;
		if (data.isButtonPressed(Move::B_TRIANGLE))
			g=data.trigger;
		if (data.isButtonPressed(Move::B_CROSS))
			b=data.trigger;
		move->getEye()->setColor(moveId,r,g,b);
	}
	if (data.isButtonPressed(Move::B_SQUARE))
		move->getMove(moveId)->setRumble(data.trigger);
}

bool Game::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if(!BaseApplication::frameRenderingQueued(evt))
		return false;
	


	//TODO: testing the move orientation
	for (int i=0; i<numMoves; i++)
	{
		Move::MoveData data = move->getMove(i)->getMoveData();

		Move::Quat moveOri = data.orientation;
		Ogre::Quaternion ori = Ogre::Quaternion(moveOri.w, moveOri.v.x, moveOri.v.y, moveOri.v.z);

		Move::Vec3 movePos = data.position;
		Ogre::Vector3 pos = Ogre::Vector3(movePos.x, movePos.y, movePos.z);

		if (cameraControl && i==0)
		{
			mCamera->setOrientation(ori);
		}
		else
		{
			mCamera->lookAt(Ogre::Vector3(0,0,0));

			moveNode[i]->setOrientation(ori);

			pos*=4.0f;
			pos.z-=800.0f;
	
			moveNode[i]->setPosition(pos);
		}
	}

	if (cameraWorks)
	{
		//showing the camera image of the Eye
		copyCameraImageToTexture();
	}

    return true;
}

void Game::copyCameraImageToTexture()
{
	int eyeX, eyeY;
	move->getEye()->getEyeDimensions(eyeX,eyeY);

	PBYTE eyeBuffer=move->getEye()->getEyeBuffer();
	if (eyeBuffer)
	{
		Ogre::HardwarePixelBufferSharedPtr buffer = camImage->getBuffer();
		buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
		const Ogre::PixelBox &pb = buffer->getCurrentLock();
		PBYTE data = static_cast<PBYTE>(pb.data);
		memcpy(data,eyeBuffer,eyeX*eyeY*4);
		buffer->unlock();
	}

	if (showMask>=0)
	{
		PBYTE maskBuffer=move->getEye()->getMaskBuffer(showMask);
		if (maskBuffer)
		{
			Ogre::HardwarePixelBufferSharedPtr buffer = maskImage->getBuffer();
			buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
			const Ogre::PixelBox &pb = buffer->getCurrentLock();
			PBYTE data = static_cast<PBYTE>(pb.data);
			memcpy(data,maskBuffer,eyeX*eyeY*2);
			buffer->unlock();
		}
	}
}

bool Game::initMove()
{
	move = Move::createDevice();

	move->subsribe(this);

	numMoves=move->initMoves();
	cameraWorks=move->initCamera(numMoves);
	

	return true;
}

