#include "stdafx.h"
#include <iostream>
#include <iomanip>

#include "Game.h"
#include "Config.h"
#include "RacketPhysicsMove.h"

void __cdecl odprintf(const char *format, ...);

Game::Game()
{
	guiInitialized=false;
	cameraWorks=false;
	cameraMode=defaultCamera;
	racketPhysics[0] = NULL;
	racketPhysics[1] = NULL;
}

Game::~Game()
{
	delete ballPhysics;
}

bool Game::configure()
{
	bool ret = BaseApplication::configure();
	
	srand(time(NULL));
	int height = mWindow->getHeight();
	int width = mWindow->getWidth();

	return ret;
}

void Game::createScene()
{
	MyGUI::LayoutManager::getInstance().loadLayout("pingpong.layout");
	initMove();

	// Table
	Ogre::Entity* tableEntity = mSceneMgr->createEntity("table", "table.mesh");
	Ogre::SceneNode* tableNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	tableNode->attachObject(tableEntity);
	tableNode->setOrientation(Ogre::Quaternion(0, 1, 0, 0) * Ogre::Quaternion(-0.707107, 0, -0.707107, 0));
	//tableNode->setPosition(0.0f, 0.50f, 0.0f);

	// Rackets
	for (int i=0; i<2; i++)
	{
		// a demo to show how skeletal animation works if joints must be moved by piece
		char tmp[20];
		sprintf_s(tmp, sizeof(tmp), "racket%d", i);
		Ogre::Entity* racketEntity = mSceneMgr->createEntity(tmp, "racket.mesh");
		racketNode[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		racketNode[i]->attachObject(racketEntity);
	}

	// Ball
	Ogre::Entity* ballEntity = mSceneMgr->createEntity("ball", "ball.mesh");
	ballNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ballNode->attachObject(ballEntity);

	Ogre::AxisAlignedBox ballBB = ballEntity->getBoundingBox();

	// Ball physics
	ballPhysics = new BallPhysics;
	//ballPhysics->setPosition(Ogre::Vector3(0.0f, 1.7f, 0.0f));
	ballPhysics->setPosition(Ogre::Vector3(0.0f, 0.78f, 0.0f));

	// Racket physics
	if (move->getMoveCount() >= 1)
	{
		racketPhysics[0] = new RacketPhysicsMove(ballPhysics, move->getMove(0));
	}
	racketPhysics[1] = new RacketPhysics(ballPhysics);

    // Set ambient light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));

    // Create a light
    Ogre::Light* l = mSceneMgr->createLight("MainLight");
    l->setPosition(20,80,50);

	initGui();
	guiInitialized=true;
}

void Game::initGui()
{
	if (numMoves<1)
	{
		mGUI->findWidget<MyGUI::TextBox>("Info")->setCaption("No device is connected. You can't control the racket.");
	}
	else if (!cameraWorks)
	{
		mGUI->findWidget<MyGUI::TextBox>("Info")->setCaption("No PS Eye found, you cant control the racket's position.");
	}
	else
	{
		mGUI->findWidget<MyGUI::TextBox>("Info")->setCaption("The PS Eye and Move are connected properly.");
	}
}

void Game::guiExitClick(MyGUI::Widget* _widget)
{
	mShutDown=true;
}

bool Game::keyPressed( const OIS::KeyEvent &arg )
{
	if (arg.key==OIS::KC_ESCAPE)
	{
		move->unsubsribe(this);
		guiInitialized=false;
	}
	return BaseApplication::keyPressed(arg);
}

void Game::moveKeyPressed(int moveId, Move::MoveButton button)
{
	if (button==Move::B_MOVE)
	{
		odprintf(">>>>>New service");
		float speedRandX = ((float)(rand() % 800 - 400)) * 0.001f;
		ballPhysics->setPosition(Ogre::Vector3(0.0f, 1.5f, -1.4f));
		ballPhysics->setSpeed(Ogre::Vector3(speedRandX, 0.0f, 1.9f));
		ballPhysics->setSpin(Ogre::Vector3(0.0f, 0.0f, 0.0f));
	}
	if (button==Move::B_SQUARE)
	{
		cameraMode++;
		if (cameraMode >= sizeof(g_Cameras) / sizeof(g_Cameras[0]))
			cameraMode = 0;

		mCamera->setPosition(g_Cameras[cameraMode].pos);
		mCamera->lookAt(g_Cameras[cameraMode].lookAt);
	}
	if (button==Move::B_CROSS)
	{
		odprintf(">>>>>CROSS");
	}
}

void Game::moveKeyReleased(int moveId, Move::MoveButton button)
{

}

void Game::moveNotify(int moveId, Move::MoveMessage message)
{

}

void Game::moveUpdated(int moveId, Move::MoveData data)
{

}

bool Game::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if(!BaseApplication::frameRenderingQueued(evt))
		return false;
	
	ballPhysics->update(evt.timeSinceLastFrame);

	for (int i=0; i<2; i++)
	{
		if (racketPhysics[i] == NULL)
			continue;

		Ogre::Vector3 pos;
		Ogre::Quaternion ori;

		racketPhysics[i]->update(evt.timeSinceLastFrame);
		racketPhysics[i]->getPosAndOri(pos, ori);

		racketNode[i]->setOrientation(ori);
		racketNode[i]->setPosition(pos);
	}

	ballNode->setPosition(ballPhysics->getPosition());

	if (!guiInitialized)
		return true;
	// Refresh GUI

    return true;
}

bool Game::initMove()
{
	move = Move::createDevice();

	move->subsribe(this);

	numMoves=move->initMoves();
	cameraWorks=move->initCamera(numMoves);

	return true;
}

