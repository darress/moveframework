#pragma once
#include "StdAfx.h"
#include "BallPhysics.h"
#include "GameLogic.h"

class GameLogic;
class BallPhysics;

class RacketPhysics
{
protected:
	GameLogic* gameLogic;
	
	int playerId;

	Ogre::Vector3 pos;
	Ogre::Vector3 speed;
	Ogre::Quaternion ori;
	Ogre::Vector3 angVel;
	bool initialized;
	BallPhysics* ball;
	float timeSinceLastHit;

public:
	RacketPhysics(BallPhysics* ball, GameLogic* gameLogic, int playerId);
	virtual ~RacketPhysics();
	
	virtual void update(float deltaT);
	virtual void makeService() = 0;

	void getPosAndOri(Ogre::Vector3& pos, Ogre::Quaternion& ori);

protected:
	void setPosAndOri(Ogre::Vector3 pos, Ogre::Quaternion ori, float deltaT);

private:
	void testCollisionWithBall();
};

