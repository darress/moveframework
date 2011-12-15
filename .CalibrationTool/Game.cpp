#include "stdafx.h"
#include "Game.h"

Game::Game()
{
	state = GameState::Normal;
}

Game::~Game()
{
}

bool Game::configure()
{
	bool ret = BaseApplication::configure();

	if (!initMove())
		return false;

	
	int height = mWindow->getHeight();
	int width = mWindow->getWidth();

	//move->PassWindowParam(width,height);

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

	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 1500, 1500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
	Ogre::Entity* entGround = mSceneMgr->createEntity("GroundEntity", "ground");
    mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entGround);

    // Set ambient light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

    // Create a light
    Ogre::Light* l = mSceneMgr->createLight("MainLight");
    l->setPosition(20,80,50);

	initGui();
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

	mGUI->findWidget<MyGUI::StaticText>("Info")->setCaption("To calibrate a device, press\n he CIRCLE button on that device!");
}

void Game::guiExitClick(MyGUI::Widget* _widget)
{
	mShutDown=true;
}

void Game::guiReadCalibrationClick(MyGUI::Widget* _widget)
{
	if (true)
	{
		MyGUI::MessagePtr pGUIMsg = MyGUI::Message::createMessageBox("Message","Fail","Could not read the calibration data.\nMake sure MotionInJoy is not installed,\nand your controller is connected via USB!",
                                MyGUI::MessageBoxStyle::IconWarning | MyGUI::MessageBoxStyle::Ok);
	}
	else
	{
		MyGUI::MessagePtr pGUIMsg = MyGUI::Message::createMessageBox("Message","Success","Calibration data successfully read.",
                                MyGUI::MessageBoxStyle::IconWarning | MyGUI::MessageBoxStyle::Ok);
	}
}


void Game::calibrationDone(int moveId)
{
	state=GameState::Normal;
	mGUI->findWidget<MyGUI::StaticText>("Info")->setCaption("Calibration done. To calibrate again,\npress the CIRCLE button on a device!");
}

void Game::moveKeyPressed(int moveId, int keyCode)
{
	if (keyCode==MoveDevice::B_CROSS)
	{
		if (state==GameState::WaitForXBeforeCalibration)
		{
			state=GameState::Calibrating;
			mGUI->findWidget<MyGUI::StaticText>("Info")->setCaption("Please rotate the Move in every possible directions,\nthen press the Cross button again.");
			move->startCalibration(0);
		}
		else if (state==GameState::Calibrating)
		{
			state=GameState::Calculating;
			mGUI->findWidget<MyGUI::StaticText>("Info")->setCaption("Please wait until the calculation process ends.");
			move->endCalibration(0);
		}
	}
	if (keyCode==MoveDevice::B_CIRCLE)
	{
		if (state==GameState::Normal)
		{
			mGUI->findWidget<MyGUI::StaticText>("Info")->setCaption("To calibrate the magnetic sensor, please press the Cross button,\nthen rotate the Move in every possible directions!");
			state=GameState::WaitForXBeforeCalibration;
		}
	}
}

void Game::moveKeyReleased(int moveId, int keyCode)
{

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
	if (numMoves==0)
	{
		return false;
	}
	move->initCamera(numMoves);

	move->subsribeMove(this);
	move->subsribeCalibration(this);
	return true;
}

