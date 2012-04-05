#include "stdafx.h"
#include "Game.h"
#include "MoveButton.h"
#include "MoveData.h"

Game::Game()
{
	state = GameState::Normal;
	guiInitialized=false;
	useMagnetometers=true;
	cameraWorks=false;
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
	MyGUI::LayoutManager::getInstance().loadLayout("main.layout");

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
			Ogre::PF_L8,     // pixel format
			Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

		//create material from texture
		Ogre::MaterialPtr lMaterial = Ogre::MaterialManager::getSingleton().create("CamMaterial",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		lMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("CameraImage");
		lMaterial->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);

		//set texture of eye image
		mGUI->findWidget<MyGUI::ImageBox>("Eye Image")->setImageTexture("CameraImage");

		//create material from texture
		Ogre::MaterialPtr lMaterial2 = Ogre::MaterialManager::getSingleton().create("MaskMaterial",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		lMaterial2->getTechnique(0)->getPass(0)->createTextureUnitState("MaskImage");
		lMaterial2->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);

		//set texture of eye image
		mGUI->findWidget<MyGUI::ImageBox>("Mask Image")->setImageTexture("MaskImage");
	}
	else
	{
		mGUI->findWidget<MyGUI::TextBox>("CameraInfo")->setCaption("No PS Eye found.");
	}
	mGUI->findWidget<MyGUI::TextBox>("PairInfo")->setCaption("To pair devices connected with USB, press P on your keyboard.");
	if (numMoves>0)
	{
		mGUI->findWidget<MyGUI::TextBox>("Info")->setCaption("To calibrate a device, press\nthe CIRCLE button on that device!");
	}
	else
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
		useMagnetometers=!useMagnetometers;
		for (int i=0; i<numMoves; i++)
		{
			move->getMove(i)->useMagnetometers(useMagnetometers);
		}
	}
	else if (arg.key==OIS::KC_ESCAPE)
	{
		move->unsubsribe(this);
		guiInitialized=false;
	}
	return BaseApplication::keyPressed(arg);
}

void Game::moveKeyPressed(int moveId, int keyCode)
{

}

void Game::moveKeyReleased(int moveId, int keyCode)
{

}

void Game::moveUpdated(int moveId, Move::MoveData data)
{
	if (!guiInitialized)
		return;
	char tmp[1000];
	sprintf(tmp,"%d devices connected.\n",numMoves);
	if (useMagnetometers)
		sprintf(tmp+ strlen(tmp), "The algorithm uses magnetometers.\n  Press M to turn it off.\n");
	else
		sprintf(tmp+ strlen(tmp), "The algorithm doesnt use magnetometers.\n  Press M to turn it on.\n");
	for (int i=0; i<numMoves; i++)
	{
		sprintf(tmp+ strlen(tmp), "\n");

		sprintf(tmp+ strlen(tmp), "The device is calibrated.\n");

		Move::Vec3 pos = data.position;
		sprintf(tmp+ strlen(tmp), "Position: %.2f %.2f %.2f\n",pos.x,pos.y,pos.z);

		Move::Quat quat = data.orientation;
		sprintf(tmp+ strlen(tmp), "Orient.: %.2f %.2f %.2f %.2f\n",quat.w,quat.v.x,quat.v.y,quat.v.z);
	}
	//mGUI->findWidget<MyGUI::TextBox>("Moves")->setCaption(tmp);
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
		moveNode[i]->setOrientation(ori);

		Move::Vec3 movePos = data.position;
		Ogre::Vector3 pos = Ogre::Vector3(movePos.x, movePos.y, movePos.z);
		pos*=4.0f;
		pos.z-=800.0f;
	
		moveNode[i]->setPosition(pos);
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

	PBYTE maskBuffer=move->getEye()->getMaskBuffer(0);
	if (maskBuffer)
	{
		Ogre::HardwarePixelBufferSharedPtr buffer = maskImage->getBuffer();
		buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
		const Ogre::PixelBox &pb = buffer->getCurrentLock();
		PBYTE data = static_cast<PBYTE>(pb.data);
		memcpy(data,maskBuffer,eyeX*eyeY);
		buffer->unlock();
	}
}

bool Game::initMove()
{
	move = Move::createDevice();

	numMoves=move->initMoves();
	cameraWorks=move->initCamera(numMoves);

	move->subsribe(this);

	return true;
}

