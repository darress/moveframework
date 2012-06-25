#pragma once
#include "BaseApplication.h"
#include "IMoveManager.h"
#include "IMoveObserver.h"
#include "MoveFactory.h"
#include "MoveData.h"

#include "stdafx.h"

class Game : public BaseApplication, Move::IMoveObserver
{
	Ogre::SceneNode* racketNode[2];

	Ogre::TexturePtr camImage;
	Ogre::TexturePtr maskImage;

	// if -1, show none, otherwise it stores the moveId
	int showMask;

	int calibratingMove;

	Move::IMoveManager* move;
	int numMoves;
	bool useMagnetometers[2];
	bool magnetometerCalibrated[2];

	bool cameraWorks;

	bool automaticColors;
	int r,g,b;

	bool cameraControl;

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
	void copyCameraImageToTexture();

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


