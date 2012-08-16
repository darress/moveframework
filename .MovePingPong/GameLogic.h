#pragma once
#include "StdAfx.h"
#include "BallPhysics.h"
#include "RacketPhysics.h"
#include "UIManager.h"

class RacketPhysics;
class BallPhysics;

class GameLogic
{
public:
	enum GameType
	{
		GAMETYPE_NOT_INITIALIZED,
		GAMETYPE_TUTORIAL,
		GAMETYPE_BEGINNER,
		GAMETYPE_INTERMEDIATE,
		GAMETYPE_EXPERT,
	};

	enum GamePhase
	{
		GAMEPHASE_SERVICE_THROWN,
		GAMEPHASE_SERVICE_RACKET_TOUCHED,
		GAMEPHASE_SERVICE_OWN_TABLE_TOUCHED,
		GAMEPHASE_GAME_RACKET_TOUCHED,
		GAMEPHASE_GAME_OTHER_TABLE_TOUCHED,
		GAMEPHASE_ROUND_OVER,
	};

private:
	Move::IMoveManager* move;
	UIManager* uiManager;

	BallPhysics* ballPhysics;
	RacketPhysics* racketPhysics[2];

	GameType gameType;
	GamePhase gamePhase;
	int playerTurn;
	int score[2];

public:
	GameLogic(Move::IMoveManager* move, UIManager* uiManager);
	~GameLogic();

	void initGame(GameType type);
	void newService();

	void update(float deltaT);

	Ogre::Vector3 getBallPosition();
	void getRacketPosAndOri(int id, Ogre::Vector3& pos, Ogre::Quaternion& ori);

	// Events
	void onTouchTable(int side);
	void onTouchRacket(int side);
	void onTouchGround();
	void onTouchNet();
	
	void onPlayerWin(int player);

	void getGameStats(GameType& gameType, GamePhase& gamePhase, int& playerTurn);
};

