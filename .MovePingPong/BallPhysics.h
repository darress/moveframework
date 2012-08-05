#pragma once
class BallPhysics
{
	Ogre::Vector3 position;
	Ogre::Vector3 speed;
	Ogre::Vector3 spin;

	Ogre::Vector3 gravity;
	bool out;

public:
	BallPhysics();
	~BallPhysics();

	void update(float deltaT);
	void setSpeed(Ogre::Vector3 speed);
	void setPosition(Ogre::Vector3 position);
	void setSpin(Ogre::Vector3 spin);
	Ogre::Vector3 getPosition();
	Ogre::Vector3 getSpeed();
};

