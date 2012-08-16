#include "StdAfx.h"
#include "RacketPhysics.h"
#include "Config.h"

void __cdecl odprintf(const char *format, ...);

RacketPhysics::RacketPhysics(BallPhysics* ball, GameLogic* gameLogic, int playerId)
	: ball(ball), gameLogic(gameLogic), playerId(playerId)
{
	pos = Ogre::Vector3::ZERO;
	speed = Ogre::Vector3::ZERO;
	ori = Ogre::Quaternion::IDENTITY;
	angVel = Ogre::Vector3::ZERO;
	initialized = false;
	timeSinceLastHit=10.0f;
}


RacketPhysics::~RacketPhysics()
{
}

void RacketPhysics::update(float deltaT)
{
	timeSinceLastHit += deltaT;
	testCollisionWithBall();
}

void RacketPhysics::getPosAndOri(Ogre::Vector3& pos, Ogre::Quaternion& ori)
{
	pos = this->pos;
	ori = this->ori;
}

void RacketPhysics::setPosAndOri(Ogre::Vector3 pos, Ogre::Quaternion ori, float deltaT)
{
	if (deltaT == 0.0f)
		return;

	if (initialized)
	{
		speed = speed * 0.7f + (pos - this->pos) / deltaT * 0.3f;
		angVel = angVel * 0.7f + (Ogre::Vector3(ori.x, ori.y, ori.z) - Ogre::Vector3(this->ori.x, this->ori.y, this->ori.z)) / deltaT * 0.3f;
	}
	this->pos = pos;
	this->ori = ori;
	initialized = true;
}

void RacketPhysics::testCollisionWithBall()
{
	if (timeSinceLastHit < 0.5f)
		return;
	Ogre::Vector3 racketNormal = Ogre::Vector3::UNIT_Y;
	racketNormal = ori * racketNormal;
	Ogre::Vector3 ballPos = ball->getPosition();
	Ogre::Plane racketPlane(racketNormal, pos);
	float distanceFromPlane = racketPlane.getDistance(ballPos);
	if (distanceFromPlane < 0.0f)
	{
		racketNormal = -racketNormal;
		distanceFromPlane = -distanceFromPlane;
	}
	//if (distanceFromPlane < g_RacketThickness + g_BallRadius)
	if (distanceFromPlane < 0.03f)
	{
		Ogre::Vector3 distanceVector = racketNormal * distanceFromPlane;
		Ogre::Vector3 nearestPointOnPlane = ballPos - distanceVector;
		Ogre::Vector3 distanceOnPlane = nearestPointOnPlane - pos;
		//odprintf("dist before: %f, %f, %f", distanceOnPlane.x, distanceOnPlane.y, distanceOnPlane.z);
		distanceOnPlane.z *= 0.5f;
		distanceOnPlane.y *= 0.5f;
		//odprintf("dist after: %f, %f, %f", distanceOnPlane.x, distanceOnPlane.y, distanceOnPlane.z);
		float projectedDistance = distanceOnPlane.length();
		//odprintf("dist sum: %f", projectedDistance);
		//if (projectedDistance < g_RacketRadiusB)
		if (projectedDistance < 0.3f)
		{
			odprintf("HIT, time sinc last hit: %f", timeSinceLastHit);
			timeSinceLastHit = 0;
			// Collision
			ballPos += distanceVector * (0.03f/abs(distanceFromPlane));
			ball->setPosition(ballPos);
			Ogre::Vector3 ballSpeed = ball->getSpeed();
			ballSpeed = ballSpeed.reflect(racketNormal);
			// Perpendicular element, used for spin
			Ogre::Vector3 speedPerp = racketPlane.projectVector(speed);
			ball->setSpin(speedPerp.crossProduct(racketNormal));
			// Parallel element, used for speed
			Ogre::Vector3 speedPara = speed - speedPerp;
			ballSpeed += speedPara * g_HitStrength;
			ball->setSpeed(ballSpeed);
			gameLogic->onTouchRacket(playerId);
		}
		else
		{
			//odprintf("no hit, time sinc last hit: %f", timeSinceLastHit);
		}
	}

}