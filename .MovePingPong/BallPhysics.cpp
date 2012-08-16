#include "StdAfx.h"
#include "BallPhysics.h"
#include "Config.h"

BallPhysics::BallPhysics(GameLogic* gameLogic)
	:gameLogic(gameLogic)
{
	gravity = Ogre::Vector3(0.0f, -9.81f, 0.0f);
	position = Ogre::Vector3(0.0f, 0.6f, 0.0f);
	speed = Ogre::Vector3::ZERO;
	spin = Ogre::Vector3::ZERO;
	out = true;
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
	if (isOverTable())
	{
		// Check collision with net
		if (abs(position.z) < g_BallRadius && 
			position.y < g_TableY + g_NetOverhang + g_BallRadius &&
			(speed.z > 0 && position.z < 0 || speed.z < 0 && position.z > 0) &&
			position.y >= g_TableY + g_BallRadius)
		{
			// If the ball hits the upper part of the net
			if (position.y > g_TableY + g_NetOverhang + g_BallRadius - 0.01f)
			{
				// If you draw some examples how to top of the net should reflect the ball, you'll see
				// you have to invert the reflected vector if the speed and the hit direction closes an 
				// angle greater than 90 grad
				Ogre::Vector3 hitPoint(position.x, g_TableY + g_NetOverhang, 0.0f);
				Ogre::Vector3 hitDirection = position - hitPoint;
				if (speed.dotProduct(hitDirection) > 0.0f)
					speed = speed.reflect(hitDirection);
				else
					speed = speed.reflect(hitDirection) * -1.0f;
			}
			// If the ball hits the side of the net
			else
			{
				speed.z = -speed.z;
			}
			gameLogic->onTouchNet();
		}
		// Check collision with table
		if (position.y < g_TableY + g_BallRadius && speed.y < 0.0f)
		{
			position.y = g_TableY + g_BallRadius;
			speed.y = -speed.y;
			gameLogic->onTouchTable(position.z > 0 ? 0 : 1);
		}
	}
	else
	{
		// If the ball touches the floor, out
		if (position.y < g_GroundY + g_BallRadius)
		{
			position.y = g_GroundY + g_BallRadius;
			out = true;
			gameLogic->onTouchGround();
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

bool BallPhysics::isOverTable()
{
	return abs(position.x) < g_TableX + g_BallRadius && abs(position.z) < g_TableZ + g_BallRadius;
}