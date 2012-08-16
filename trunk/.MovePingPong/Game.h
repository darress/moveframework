#pragma once
#include "BaseApplication.h"
#include "stdafx.h"
#include "GameLogic.h"
#include "UIManager.h"

class Game : public BaseApplication, Move::IMoveObserver
{
	Ogre::SceneNode* racketNode[2];
	Ogre::SceneNode* ballNode;

	GameLogic* gameLogic;
	UIManager* uiManager;

	Move::IMoveManager* move;
	int numMoves;

	bool cameraWorks;

	int cameraMode;

	bool menuMode;
	Ogre::Quaternion initOri;

	bool guiInitialized;

public:
    Game(void);
    virtual ~Game(void);

protected:
    virtual void createScene(void);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool configure();
	bool keyPressed( const OIS::KeyEvent &arg );

	bool initMove();

private:
	void initGui();
	void guiExitClick(MyGUI::Widget* _widget);
	void guiReadCalibrationClick(MyGUI::Widget* _widget);
	void guiCalibrateClick(MyGUI::Widget* _widget);

public:
	void moveKeyPressed(int moveId, Move::MoveButton button);
	void moveKeyReleased(int moveId, Move::MoveButton button);
	void moveUpdated(int moveId, Move::MoveData data);
	void moveNotify(int moveId, Move::MoveMessage message);

	void onMenuClicked(MyGUI::Widget* _widget);
};


