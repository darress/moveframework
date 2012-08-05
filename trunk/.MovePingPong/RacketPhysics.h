#pragma once
#include "StdAfx.h"
#include "BallPhysics.h"

class RacketPhysics
{
	Ogre::Vector3 pos;
	Ogre::Vector3 speed;
	Ogre::Quaternion ori;
	Ogre::Vector3 angVel;
	bool initialized;
	BallPhysics* ball;
	float timeSinceLastHit;

public:
	RacketPhysics(BallPhysics* ball);
	virtual ~RacketPhysics();
	
	virtual void update(float deltaT);
	void getPosAndOri(Ogre::Vector3& pos, Ogre::Quaternion& ori);

protected:
	void setPosAndOri(Ogre::Vector3 pos, Ogre::Quaternion ori, float deltaT);

private:
	void testCollisionWithBall();
};

