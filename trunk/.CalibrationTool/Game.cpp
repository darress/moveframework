#include "stdafx.h"
#include "Game.h"

Game::Game()
{
	state = GameState::Normal;
	guiInitialized=false;
	useMagnetometers=true;
}

Game::~Game()
{
}

bool Game::configure()
{
	bool ret = BaseApplication::configure();
	
	initMove();

	int height = mWindow->getHeight();
	int width = mWindow->getWidth();

	return ret;
}

void Game::createScene()
{
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
	MyGUI::LanguageManager::getInstance().loadUserTags("core_theme_black_orange_tag.xml");
	mGUI->load("core_skin.xml");

	MyGUI::VectorWidgetPtr& root = MyGUI::LayoutManager::getInstance().load("main.layout");

	int eyeX, eyeY;
	move->getEyeDimensions(eyeX,eyeY);

	//create texture for camera image
	camImage = Ogre::TextureManager::getSingleton().createManual(
		"CameraImage", // name
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,      // type
		eyeX, eyeY,         // width & height
		0,                // number of mipmaps
		Ogre::PF_B8G8R8A8,     // pixel format
		Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

	//create material from texture
	Ogre::MaterialPtr lMaterial = Ogre::MaterialManager::getSingleton().create("CamMaterial",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	lMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("CameraImage");
	lMaterial->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);

	//set texture of eye image
	mGUI->findWidget<MyGUI::StaticImage>("Eye Image")->setImageTexture("CameraImage");

	mGUI->findWidget<MyGUI::StaticText>("PairInfo")->setCaption("To pair devices connected with USB, press P on your keyboard.");
	if (numMoves>0)
	{
		mGUI->findWidget<MyGUI::StaticText>("Info")->setCaption("To calibrate a device, press\nthe CIRCLE button on that device!");
	}
	else
	{
		mGUI->findWidget<MyGUI::StaticText>("Info")->setCaption("No device is connected via Bluetooth. Nothing to calibrate.");
		mGUI->findWidget<MyGUI::StaticText>("Moves")->setCaption("No device is found.");
	}
}

void Game::guiExitClick(MyGUI::Widget* _widget)
{
	mShutDown=true;
}

void Game::calibrationDone(int moveId)
{
	state=GameState::Normal;
	mGUI->findWidget<MyGUI::StaticText>("Info")->setCaption("Calibration done. To calibrate again,\npress the CIRCLE button on a device!");
}

bool Game::keyPressed( const OIS::KeyEvent &arg )
{
	if (arg.key == OIS::KC_P)
    {
		try
		{
			int num = move->pairMoves();
			if (num==0)
			{
				mGUI->findWidget<MyGUI::StaticText>("PairInfo")->setCaption("No new device has been paired, to try again, press P.");
			}
			else
			{
				char tmp[100];
				sprintf(tmp,"%d new devices has been paired, to pair again, press P.",num);
				mGUI->findWidget<MyGUI::StaticText>("PairInfo")->setCaption(tmp);
			}
		}
		catch(MoveNoBTDongleFound)
		{
			mGUI->findWidget<MyGUI::StaticText>("PairInfo")->setCaption("No Bluetooth dongle found, please connect one!");
		}
	}
	else if (arg.key==OIS::KC_M)
	{
		useMagnetometers=!useMagnetometers;
		for (int i=0; i<numMoves; i++)
		{
			move->useMagnetometer(i,useMagnetometers);
		}
	}
	else if (arg.key==OIS::KC_ESCAPE)
	{
		move->unsubsribeMove(this);
		move->unsubsribeCalibration(this);
		guiInitialized=false;
	}
	return BaseApplication::keyPressed(arg);
}

void Game::moveKeyPressed(int moveId, int keyCode)
{
	if (keyCode==MoveDevice::B_CROSS && calibratingMove==moveId)
	{
		if (state==GameState::WaitForXBeforeCalibration)
		{
			state=GameState::Calibrating;
			mGUI->findWidget<MyGUI::StaticText>("Info")->setCaption("Please rotate the Move in every possible directions,\nthen press the Cross button again.");
			move->startCalibration(moveId);
		}
		else if (state==GameState::Calibrating)
		{
			state=GameState::Calculating;
			mGUI->findWidget<MyGUI::StaticText>("Info")->setCaption("Please wait until the calculation process ends.");
			move->endCalibration(moveId);
		}
	}
	if (keyCode==MoveDevice::B_CIRCLE)
	{
		if (state==GameState::Normal)
		{
			mGUI->findWidget<MyGUI::StaticText>("Info")->setCaption("To calibrate the magnetic sensor, please press the Cross button,\nthen rotate the Move in every possible directions!");
			state=GameState::WaitForXBeforeCalibration;
			calibratingMove=moveId;
		}
	}
}

void Game::moveKeyReleased(int moveId, int keyCode)
{

}

void Game::moveUpdated(int moveId, Move::MoveData data)
{
	if (!guiInitialized)
		return;
	MyGUI::UString strin;
	char tmp[100];
	sprintf(tmp,"%d devices connected.\n",numMoves);
	strin.append(tmp);
	if (useMagnetometers)
		strin.append("The algorithm uses magnetometers.\n  Press M to turn it off.\n");
	else
		strin.append("The algorithm doesnt use magnetometers.\n  Press M to turn it on.\n");
	for (int i=0; i<numMoves; i++)
	{
		strin.append("\n");
		if (!move->isCalibrated(i))
		{
			strin.append("The device is not calibrated.\n");
		}
		else
		{
			strin.append("The device is calibrated.\n");

			Move::Vector3 pos = move->getPosition(i);
			sprintf(tmp,"Position: %.2f %.2f %.2f\n",pos.x,pos.y,pos.z);
			strin.append(tmp);

			Move::Quaternion quat = move->getOrientation(i);
			sprintf(tmp,"Orient.: %.2f %.2f %.2f %.2f\n",quat.w,quat.x,quat.y,quat.z);
			strin.append(tmp);
		}
	}
	mGUI->findWidget<MyGUI::StaticText>("Moves")->setCaption(strin);
}

bool Game::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if(!BaseApplication::frameRenderingQueued(evt))
		return false;
	


	//TODO: testing the move orientation
	for (int i=0; i<numMoves; i++)
	{
		Move::Quaternion moveOri = move->getOrientation(i);
		Ogre::Quaternion ori = Ogre::Quaternion(moveOri.w, moveOri.x, moveOri.y, moveOri.z);
		moveNode[i]->setOrientation(ori);

		Move::Vector3 movePos = move->getPosition(i);
		Ogre::Vector3 pos = Ogre::Vector3(movePos.x, movePos.y, movePos.z);
		pos*=4.0f;
		pos.z-=800.0f;
	
		moveNode[i]->setPosition(pos);
	}

	//showing the camera image of the Eye
	copyCameraImageToTexture(camImage);

    return true;
}

void Game::copyCameraImageToTexture(Ogre::TexturePtr texture)
{
	int eyeX, eyeY;
	move->getEyeDimensions(eyeX,eyeY);

	PBYTE eyeBuffer=move->getEyeBuffer();
	if (eyeBuffer)
	{
		Ogre::HardwarePixelBufferSharedPtr buffer = texture->getBuffer(0,0);
		buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
		const Ogre::PixelBox &pb = buffer->getCurrentLock();
		PBYTE data = static_cast<PBYTE>(pb.data);
		memcpy(data,eyeBuffer,eyeX*eyeY*4);

		for (size_t j = 0; j < eyeX; j++)
			for(size_t i = 0; i < eyeY; i++)
			{
				*data++; // B
				*data++; // G
				*data++; // R
				*data++ = 255; // A
			}
		buffer->unlock();
	}
}

bool Game::initMove()
{
	move = Move::createDevice();

	numMoves=move->initMoves();
	move->initCamera(numMoves);

	move->subsribeMove(this);
	move->subsribeCalibration(this);

	return true;
}

