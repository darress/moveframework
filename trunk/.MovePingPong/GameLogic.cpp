#include "StdAfx.h"
#include "GameLogic.h"
#include "RacketPhysicsMove.h"
#include "RacketPhysicsAI.h"

void __cdecl odprintf(const char *format, ...);

GameLogic::GameLogic(Move::IMoveManager* move, UIManager* uiManager)
	:move(move), uiManager(uiManager)
{
	ballPhysics = NULL;
	racketPhysics[0] = NULL;
	racketPhysics[1] = NULL;
	gameType = GAMETYPE_NOT_INITIALIZED;
	score[0] = score[1] = 0;
}


GameLogic::~GameLogic()
{
	if (ballPhysics != NULL)
		delete ballPhysics;
	if (racketPhysics[0] != NULL)
		delete racketPhysics[0];
	if (racketPhysics[1] != NULL)
		delete racketPhysics[1];
}

void GameLogic::initGame(GameType type)
{
	// delete previous objects
	if (ballPhysics != NULL)
		delete ballPhysics;
	if (racketPhysics[0] != NULL)
		delete racketPhysics[0];
	if (racketPhysics[1] != NULL)
		delete racketPhysics[1];

	gameType = type;
	score[0] = score[1] = 0;

	if (gameType == GAMETYPE_TUTORIAL ||
		gameType == GAMETYPE_BEGINNER ||
		gameType == GAMETYPE_INTERMEDIATE ||
		gameType == GAMETYPE_EXPERT)
	{
		// Ball physics
		ballPhysics = new BallPhysics(this);

		// Racket physics
		if (move->getMoveCount() >= 1)
		{
			racketPhysics[0] = new RacketPhysicsMove(ballPhysics, this, 0, move->getMove(0));
		}
		racketPhysics[1] = new RacketPhysicsAI(ballPhysics, this, 1);

		uiManager->updateScore(score[0], score[1]);

		switch(gameType)
		{
		case GAMETYPE_TUTORIAL:
			uiManager->showGameType("Tutorial Game");
			break;
		case GAMETYPE_BEGINNER:
			uiManager->showGameType("Beginner Game");
			break;
		case GAMETYPE_INTERMEDIATE:
			uiManager->showGameType("Intermediate Game");
			break;
		case GAMETYPE_EXPERT:
			uiManager->showGameType("Expert Game");
			break;
		}
	}
}

void GameLogic::newService()
{
	if (gameType == GAMETYPE_TUTORIAL ||
		gameType == GAMETYPE_BEGINNER ||
		gameType == GAMETYPE_INTERMEDIATE ||
		gameType == GAMETYPE_EXPERT)
	{
		odprintf(">>>>>New service");
		uiManager->showInfo("New service", 1.0f);

		// the enemy give a service
		racketPhysics[1]->makeService();
		gamePhase = GAMEPHASE_SERVICE_RACKET_TOUCHED;
		playerTurn = 1;
	}
}

void GameLogic::update(float deltaT)
{
	if (ballPhysics != NULL)
	{
		ballPhysics->update(deltaT);
	}

	for (int i=0; i<2; i++)
	{
		if (racketPhysics[i] == NULL)
			continue;

		Ogre::Vector3 pos;
		Ogre::Quaternion ori;

		racketPhysics[i]->update(deltaT);
		racketPhysics[i]->getPosAndOri(pos, ori);
	}
}

Ogre::Vector3 GameLogic::getBallPosition()
{
	if (ballPhysics != NULL)
		return ballPhysics->getPosition();
	return Ogre::Vector3(0.0f, 0.5f, 0.0f);
}

void GameLogic::getRacketPosAndOri(int id, Ogre::Vector3& pos, Ogre::Quaternion& ori)
{
	if (racketPhysics[id] != NULL)
	{
		racketPhysics[id]->getPosAndOri(pos, ori);
	}
	else
	{
		pos = Ogre::Vector3(0.0f, 0.5f, 0.0f);
		ori = Ogre::Quaternion::IDENTITY;
	}
}

// Events
void GameLogic::onTouchTable(int side)
{
	odprintf("onTouchTable %d %d", playerTurn, side);
	switch(gamePhase)
	{
	case GAMEPHASE_SERVICE_THROWN:
		onPlayerWin(1 - playerTurn);
		return;
	case GAMEPHASE_SERVICE_RACKET_TOUCHED:
		if (side == playerTurn)
			gamePhase = GAMEPHASE_SERVICE_OWN_TABLE_TOUCHED;
		else
			onPlayerWin(1 - playerTurn);
		return;
	case GAMEPHASE_SERVICE_OWN_TABLE_TOUCHED:
		if (side == playerTurn)
			onPlayerWin(1 - playerTurn);
		else
		{
			gamePhase = GAMEPHASE_GAME_OTHER_TABLE_TOUCHED;
			playerTurn = 1 - playerTurn;
			odprintf("GAMEPHASE_SERVICE_OWN_TABLE_TOUCHED %d", playerTurn);
		}
		return;
	case GAMEPHASE_GAME_RACKET_TOUCHED:
		if (side == playerTurn)
			onPlayerWin(1 - playerTurn);
		else
		{
			gamePhase = GAMEPHASE_GAME_OTHER_TABLE_TOUCHED;
			playerTurn = 1 - playerTurn;
			odprintf("GAMEPHASE_GAME_RACKET_TOUCHED %d", playerTurn);
		}
		return;
	case GAMEPHASE_GAME_OTHER_TABLE_TOUCHED:
		onPlayerWin(1 - playerTurn);
		return;
	}
}

void GameLogic::onTouchRacket(int side)
{
	odprintf("onTouchRacket %d %d", playerTurn, side);
	switch(gamePhase)
	{
	case GAMEPHASE_SERVICE_THROWN:
		if (side == playerTurn)
			gamePhase = GAMEPHASE_SERVICE_RACKET_TOUCHED;
		else
			onPlayerWin(playerTurn);
		return;
	case GAMEPHASE_SERVICE_RACKET_TOUCHED:
		onPlayerWin(1 - playerTurn);
		return;
	case GAMEPHASE_SERVICE_OWN_TABLE_TOUCHED:
		if (side == playerTurn)
			onPlayerWin(1 - playerTurn);
		else
		{
			if (ballPhysics->isOverTable())
				onPlayerWin(playerTurn);
			else
				onPlayerWin(1 - playerTurn);
		}
		return;
	case GAMEPHASE_GAME_RACKET_TOUCHED:
		onPlayerWin(1 - playerTurn);
		return;
	case GAMEPHASE_GAME_OTHER_TABLE_TOUCHED:
		if (side == playerTurn)
			gamePhase = GAMEPHASE_GAME_RACKET_TOUCHED;
		else
			onPlayerWin(playerTurn);
		return;
	}
}

void GameLogic::onTouchGround()
{
	odprintf("onTouchGround %d", playerTurn);
	if (gamePhase != GAMEPHASE_ROUND_OVER)
		onPlayerWin(1 - playerTurn);
}

void GameLogic::onTouchNet()
{
	odprintf("onTouchNet %d", playerTurn);
}

void GameLogic::onPlayerWin(int player)
{
	odprintf("onPlayerWin %d", playerTurn, player);
	gamePhase = GAMEPHASE_ROUND_OVER;

	score[player]++;
	uiManager->updateScore(score[0], score[1]);

	if (player == 0)
		uiManager->showInfo("You won a point!", 4.0f);
	else
		uiManager->showInfo("The enemy won a point!", 4.0f);
}

void GameLogic::getGameStats(GameType& gameType, GamePhase& gamePhase, int& playerTurn)
{
	gameType = this->gameType;
	gamePhase = this->gamePhase;
	playerTurn = this->playerTurn;
}