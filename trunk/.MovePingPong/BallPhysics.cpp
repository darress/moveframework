#include "StdAfx.h"
#include "BallPhysics.h"
#include "Config.h"

BallPhysics::BallPhysics()
{
	gravity = Ogre::Vector3(0.0f, -10.0f, 0.0f);
	position = Ogre::Vector3::ZERO;
	speed = Ogre::Vector3::ZERO;
	spin = Ogre::Vector3::ZERO;
	out = false;
}


BallPhysics::~BallPhysics()
{
}

void BallPhysics::update(float deltaT)
{
	if (out == true)
		return;
	// Gravity
	speed += gravity * deltaT;
	// Spin
	speed += spin.crossProduct(speed) * g_SpinFactor * deltaT;
	// Air resistance
	Ogre::Vector3 speedNorm = speed;
	speedNorm.normalise();
	speed += speedNorm * -speed.squaredLength() * g_AirResistance * deltaT;

	position += speed * deltaT;
	// If the ball is over the table
	if (abs(position.x) < g_TableX + g_BallRadius && abs(position.z) < g_TableZ + g_BallRadius)
	{
		// Check collision with table
		if (position.y < g_TableY + g_BallRadius && speed.y < 0.0f)
		{
			position.y = g_TableY + g_BallRadius;
			speed.y = -speed.y;
		}
	}
	else
	{
		// If the ball touches the floor, out
		if (position.y < g_GroundY + g_BallRadius)
		{
			position.y = g_GroundY + g_BallRadius;
			out = true;
		}
	}
}

void BallPhysics::setSpeed(Ogre::Vector3 speed)
{
	this->speed = speed;
}

void BallPhysics::setPosition(Ogre::Vector3 position)
{
	this->position = position;
	out = false;
}

void BallPhysics::setSpin(Ogre::Vector3 spin)
{
	this->spin = spin;
}

Ogre::Vector3 BallPhysics::getPosition()
{
	return position;
}

Ogre::Vector3 BallPhysics::getSpeed()
{
	return speed;
}
