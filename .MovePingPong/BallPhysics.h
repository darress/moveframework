#pragma once

#include "GameLogic.h"

class GameLogic;

class BallPhysics
{
	GameLogic* gameLogic;

	Ogre::Vector3 position;
	Ogre::Vector3 speed;
	Ogre::Vector3 spin;

	Ogre::Vector3 gravity;
	bool out;

public:
	BallPhysics(GameLogic* gameLogic);
	~BallPhysics();

	void update(float deltaT);
	void setSpeed(Ogre::Vector3 speed);
	void setPosition(Ogre::Vector3 position);
	void setSpin(Ogre::Vector3 spin);
	Ogre::Vector3 getPosition();
	Ogre::Vector3 getSpeed();

	bool isOverTable();
};

