#pragma once
#include "StdAfx.h"
#include "racketphysics.h"

class RacketPhysicsMove :
	public RacketPhysics
{
	Move::IMoveController* move;

public:
	RacketPhysicsMove(BallPhysics* ball, GameLogic* gameLogic, int playerId, Move::IMoveController* move);
	virtual ~RacketPhysicsMove();

	void update(float deltaT);
	void makeService();
};

