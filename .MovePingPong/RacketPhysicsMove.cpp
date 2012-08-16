#include "StdAfx.h"
#include "RacketPhysicsMove.h"


RacketPhysicsMove::RacketPhysicsMove(BallPhysics* ball, GameLogic* gameLogic, int playerId, Move::IMoveController* move)
	:RacketPhysics(ball, gameLogic, playerId), move(move)
{

}


RacketPhysicsMove::~RacketPhysicsMove()
{
}

void RacketPhysicsMove::update(float deltaT)
{
	Move::MoveData data = move->getMoveData();

	Move::Quat moveOri = data.orientation;
	Ogre::Quaternion ori = Ogre::Quaternion(moveOri.w, moveOri.v.x, moveOri.v.y, moveOri.v.z);

	Move::Vec3 movePos = data.position;
	Ogre::Vector3 pos = Ogre::Vector3(movePos.x, movePos.y, movePos.z);

	pos*=0.01f;
	pos.z*=1.0f;
	pos.z+=1.5f;
	pos.y+=1.1f;

	setPosAndOri(pos, ori, deltaT);
	RacketPhysics::update(deltaT);
}

void RacketPhysicsMove::makeService()
{

}