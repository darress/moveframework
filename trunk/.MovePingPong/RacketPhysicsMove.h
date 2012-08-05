#pragma once
#include "StdAfx.h"
#include "racketphysics.h"

class RacketPhysicsMove :
	public RacketPhysics
{
	Move::IMoveController* move;

public:
	RacketPhysicsMove(BallPhysics* ball, Move::IMoveController* move);
	virtual ~RacketPhysicsMove();

	void update(float deltaT);
};

