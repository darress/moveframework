#pragma once

#include "StdAfx.h"
#include "RacketPhysics.h"

class RacketPhysicsAI : public RacketPhysics
{
public:
	RacketPhysicsAI(BallPhysics* ball, GameLogic* gameLogic, int playerId);
	virtual ~RacketPhysicsAI();

	void update(float deltaT);
	void makeService();
};

