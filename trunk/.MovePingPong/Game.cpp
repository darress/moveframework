#include "stdafx.h"
#include <iostream>
#include <iomanip>

#include "Game.h"
#include "Config.h"

void __cdecl odprintf(const char *format, ...);

Game::Game()
{
	guiInitialized=false;
	cameraWorks=false;
	cameraMode=defaultCamera;
	gameLogic = NULL;
	uiManager = NULL;
	menuMode = false;
}

Game::~Game()
{
	if (gameLogic != NULL)
		delete gameLogic;
	if (uiManager != NULL)
		delete uiManager;
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
	mGUI->findWidget<MyGUI::Button>("Menu Tutorial")->eventMouseButtonClick = MyGUI::newDelegate(this, &Game::onMenuClicked); 
	mGUI->findWidget<MyGUI::Button>("Menu Beginner")->eventMouseButtonClick = MyGUI::newDelegate(this, &Game::onMenuClicked); 
	mGUI->findWidget<MyGUI::Button>("Menu Intermediate")->eventMouseButtonClick = MyGUI::newDelegate(this, &Game::onMenuClicked); 
	mGUI->findWidget<MyGUI::Button>("Menu Expert")->eventMouseButtonClick = MyGUI::newDelegate(this, &Game::onMenuClicked); 
	mGUI->findWidget<MyGUI::Button>("Menu Exit")->eventMouseButtonClick = MyGUI::newDelegate(this, &Game::onMenuClicked); 
	mGUI->findWidget<MyGUI::Canvas>("Main Menu")->setVisible(false);
	mGUI->findWidget<MyGUI::TextBox>("Score")->setCaption("");
	mGUI->findWidget<MyGUI::TextBox>("Game Type")->setCaption("Press Triangle for Menu");
	MyGUI::PointerManager::getInstance().setVisible(false);
	initMove();

	// Table
	Ogre::Entity* tableEntity = mSceneMgr->createEntity("table", "table.mesh");
	Ogre::SceneNode* tableNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	tableNode->attachObject(tableEntity);
	tableNode->setOrientation(Ogre::Quaternion(0, 1, 0, 0) * Ogre::Quaternion(-0.707107, 0, -0.707107, 0));

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

    // Set ambient light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));

    // Create a light
    Ogre::Light* l = mSceneMgr->createLight("MainLight");
    l->setPosition(20,80,50);

	initGui();
	guiInitialized=true;

	gameLogic = new GameLogic(move, uiManager);
}

void Game::initGui()
{
	uiManager = new UIManager(mGUI);
	if (numMoves<1)
	{
		uiManager->showInfo("No device is connected.", 10.0f);
	}
	else if (!cameraWorks)
	{
		uiManager->showInfo("No PS Eye found.", 10.0f);
	}
	else
	{
		uiManager->showInfo("All controls connected.", 10.0f);
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
		gameLogic->newService();
	}
	if (button==Move::B_SQUARE)
	{
		cameraMode++;
		if (cameraMode >= sizeof(g_Cameras) / sizeof(g_Cameras[0]))
			cameraMode = 0;

		mCamera->setPosition(g_Cameras[cameraMode].pos);
		mCamera->lookAt(g_Cameras[cameraMode].lookAt);
	}
	if (button==Move::B_TRIANGLE)
	{
		menuMode = true;

		Move::MoveData data = move->getMove(0)->getMoveData();

		Move::Quat moveOri = data.orientation;
		initOri = Ogre::Quaternion(moveOri.w, moveOri.v.x, moveOri.v.y, moveOri.v.z);

		mGUI->findWidget<MyGUI::Canvas>("Main Menu")->setVisible(true);
		MyGUI::PointerManager::getInstance().setVisible(true);
	}
	if (button==Move::B_CROSS)
	{
		odprintf(">>>>>CROSS");
	}
}

void Game::moveKeyReleased(int moveId, Move::MoveButton button)
{
	if (button==Move::B_TRIANGLE)
	{
		Move::MoveData data = move->getMove(0)->getMoveData();

		Move::Quat moveOri = data.orientation;
		Ogre::Quaternion ori = Ogre::Quaternion(moveOri.w, moveOri.v.x, moveOri.v.y, moveOri.v.z);

		if (ori.Dot(initOri))
			ori = Ogre::Quaternion(-ori.w, -ori.x, -ori.y, -ori.z);

		Ogre::Quaternion relOriQ = (ori.Inverse() * initOri);
		Ogre::Vector3 relOri(relOriQ.x, relOriQ.y, relOriQ.z);
		relOri *= 1500.0f;

		MyGUI::InputManager::getInstance().injectMouseRelease(-relOri.y + mWindow->getWidth()*0.5f, -relOri.x + mWindow->getHeight()*0.5f, MyGUI::MouseButton::Enum(0));
		menuMode = false;

		mGUI->findWidget<MyGUI::Canvas>("Main Menu")->setVisible(false);
		MyGUI::PointerManager::getInstance().setVisible(false);
	}
}

void Game::moveNotify(int moveId, Move::MoveMessage message)
{

}

void Game::moveUpdated(int moveId, Move::MoveData data)
{

}

void Game::onMenuClicked(MyGUI::Widget* widget)
{
	if (widget->getName().compare("Menu Tutorial") == 0)
	{
		gameLogic->initGame(GameLogic::GAMETYPE_TUTORIAL);
	}
	else if (widget->getName().compare("Menu Beginner") == 0)
	{
		gameLogic->initGame(GameLogic::GAMETYPE_BEGINNER);
	}
	else if (widget->getName().compare("Menu Intermediate") == 0)
	{
		gameLogic->initGame(GameLogic::GAMETYPE_INTERMEDIATE);
	}
	else if (widget->getName().compare("Menu Expert") == 0)
	{
		gameLogic->initGame(GameLogic::GAMETYPE_EXPERT);
	}
	else if (widget->getName().compare("Menu Exit") == 0)
	{
		mShutDown = true;
	}
}

bool Game::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if(!BaseApplication::frameRenderingQueued(evt))
		return false;
	
	gameLogic->update(evt.timeSinceLastFrame);
	uiManager->update(evt.timeSinceLastFrame);

	ballNode->setPosition(gameLogic->getBallPosition());

	for (int i=0; i<2; i++)
	{
		Ogre::Vector3 pos;
		Ogre::Quaternion ori;

		gameLogic->getRacketPosAndOri(i, pos, ori);

		racketNode[i]->setOrientation(ori);
		racketNode[i]->setPosition(pos);
	}

	if (!guiInitialized)
		return true;
	// Refresh GUI
	if (menuMode)
	{
		Move::MoveData data = move->getMove(0)->getMoveData();

		Move::Quat moveOri = data.orientation;
		Ogre::Quaternion ori = Ogre::Quaternion(moveOri.w, moveOri.v.x, moveOri.v.y, moveOri.v.z);

		if (ori.Dot(initOri))
			ori = Ogre::Quaternion(-ori.w, -ori.x, -ori.y, -ori.z);

		Ogre::Quaternion relOriQ = (ori.Inverse() * initOri);
		Ogre::Vector3 relOri(relOriQ.x, relOriQ.y, relOriQ.z);
		relOri *= 1500.0f;

		MyGUI::InputManager::getInstance().injectMouseMove(-relOri.y + mWindow->getWidth()*0.5f, -relOri.x + mWindow->getHeight()*0.5f, 0.0f);

	}

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

