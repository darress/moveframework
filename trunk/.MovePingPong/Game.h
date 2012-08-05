#pragma once
#include "BaseApplication.h"
#include "BallPhysics.h"
#include "RacketPhysics.h"

#include "stdafx.h"

class Game : public BaseApplication, Move::IMoveObserver
{
	Ogre::SceneNode* racketNode[2];
	Ogre::SceneNode* ballNode;

	BallPhysics* ballPhysics;
	RacketPhysics* racketPhysics[2];

	Move::IMoveManager* move;
	int numMoves;

	bool cameraWorks;

	int cameraMode;

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
};


