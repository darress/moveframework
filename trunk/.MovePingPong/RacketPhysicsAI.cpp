#include "StdAfx.h"
#include "RacketPhysicsAI.h"
#include "Config.h"

RacketPhysicsAI::RacketPhysicsAI(BallPhysics* ball, GameLogic* gameLogic, int playerId)
	: RacketPhysics(ball, gameLogic, playerId)
{
}


RacketPhysicsAI::~RacketPhysicsAI()
{
}

void RacketPhysicsAI::update(float deltaT)
{
	GameLogic::GameType gameType;
	GameLogic::GamePhase gamePhase;
	int playerTurn;
	gameLogic->getGameStats(gameType, gamePhase, playerTurn);

	// The AI racket should follow the ball in each mode
	Ogre::Vector3 ballPos = ball->getPosition();
	Ogre::Vector3 newPos(ballPos.x, 1.5f, -1.5f);
	pos = Ogre::Vector3(newPos);

	// if it's a tutorial game
	if (gameType == GameLogic::GAMETYPE_BEGINNER)
	{
		// If it's our turn and the ball already bounced at our side
		if (playerTurn == playerId && gamePhase == GameLogic::GAMEPHASE_GAME_OTHER_TABLE_TOUCHED)
		{
			// We hit back the ball 40 cm above the table
			if (ballPos.y > g_TableY + 0.4f)
			{
				//TODO: refine these
				Ogre::Vector3 destination(0.0f, 0.5f, g_TableY);
				float v0y = 1.5f;

				Ogre::Vector3 direction = destination - ballPos;
				Ogre::Vector3 projectedDir(direction.x, 0.0f, direction.z);
				
				// Ballistic trajectory
				float x = projectedDir.length();
				float y = direction.y;
				float g = g_Gravity;
				float t = v0y / g + sqrt((v0y * v0y) / (g * g) - (2 * y) / g);
				float v0x = x / t;

				Ogre::Vector3 newSpeed = projectedDir;
				newSpeed.normalise();
				newSpeed *= v0x;
				newSpeed.y = v0y;
				ball->setSpeed(newSpeed);
				ball->setSpin(Ogre::Vector3::ZERO);

				gameLogic->onTouchRacket(playerId);
			}
		}
	}
}

void RacketPhysicsAI::makeService()
{
	GameLogic::GameType gameType;
	GameLogic::GamePhase gamePhase;
	int playerTurn;
	gameLogic->getGameStats(gameType, gamePhase, playerTurn);

	if (gameType == GameLogic::GAMETYPE_TUTORIAL)
	{
		ball->setPosition(Ogre::Vector3(0.0f, 1.2f, -1.3f));
		ball->setSpeed(Ogre::Vector3(0.0f, 0.0f, 2.4f));
		ball->setSpin(Ogre::Vector3(0.0f, 0.0f, 0.0f));
	}
	if (gameType == GameLogic::GAMETYPE_BEGINNER)
	{
		float speedRandX = ((float)(rand() % 800 - 400)) * 0.001f;
		ball->setPosition(Ogre::Vector3(0.0f, 1.2f, -1.6f));
		ball->setSpeed(Ogre::Vector3(0, 0.0f, 2.4f));
		ball->setSpin(Ogre::Vector3(0.0f, 0.0f, 0.0f));
	}
}