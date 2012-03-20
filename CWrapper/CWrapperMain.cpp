
#include "MovePrecompiled.h"
#include "IMoveManager.h"
#include "MoveFactory.h"
#include "MoveData.h"

Move::IMoveManager* move;
int numMoves;

typedef struct tagVector3
{ 
	float x;
	float y;
	float z;
} Vector3; 

typedef struct tagQuaternion
{ 
	float w;
	float x;
	float y;
	float z;
} Quaternion; 

typedef void (__stdcall *UPDATE_CALLBACK)(int id, Vector3 position, Quaternion orientation, int trigger);
UPDATE_CALLBACK registered_callback;

typedef void (__stdcall *KEY_CALLBACK)(int id, int keyCode);
KEY_CALLBACK registered_keydown;
KEY_CALLBACK registered_keyup;

class MoveObserver : public Move::IMoveObserver
{
	void moveKeyPressed(int moveId, int keyCode)
	{
		(*registered_keydown)(moveId, keyCode);
	}

	void moveKeyReleased(int moveId, int keyCode)
	{
		(*registered_keyup)(moveId, keyCode);
	}

	void moveUpdated(int moveId, Move::MoveData data)
	{
		Vector3 pos;
		pos.x=data.position.x;
		pos.y=data.position.y;
		pos.z=data.position.z;

		Quaternion q;
		q.w=data.orientation.w;
		q.x=data.orientation.x;
		q.y=data.orientation.y;
		q.z=data.orientation.z;

		(*registered_callback)(moveId, pos, q, data.trigger);
	}
};
MoveObserver* observer=new MoveObserver;

extern "C" __declspec(dllexport) void __stdcall init() 
{
		move = Move::createDevice();

		numMoves=move->initMoves();
		move->initCamera(numMoves);
}

extern "C" __declspec(dllexport) int __stdcall getMovesCount() 
{
	return numMoves;
}

extern "C" __declspec(dllexport) void __stdcall subscribeMove(UPDATE_CALLBACK updateCallback, KEY_CALLBACK keyDownCallback, KEY_CALLBACK keyUpCallback) 
{
	registered_callback=updateCallback;
	registered_keyup=keyUpCallback;
	registered_keydown=keyDownCallback;
	move->subsribeMove(observer);
}

extern "C" __declspec(dllexport) void __stdcall unsubscribeMove() 
{
	move->unsubsribeMove(observer);
}

extern "C" __declspec(dllexport) Quaternion __stdcall getOrientation(int id) 
{
	Move::Quaternion ori = move->getOrientation(id);
	Quaternion q;
	q.w=ori.w;
	q.x=ori.x;
	q.y=ori.y;
	q.z=ori.z;
	return q;
}

extern "C" __declspec(dllexport) Vector3 __stdcall getPosition(int id) 
{
	Move::Vector3 pos = move->getPosition(id);
	Vector3 v;
	v.x=pos.x;
	v.y=pos.y;
	v.z=pos.z;
	return v;
}

extern "C" __declspec(dllexport) bool __stdcall getButtonState(int id, int keyId) 
{
	return move->getButtonState(id, keyId);
}

extern "C" __declspec(dllexport) int __stdcall getTriggerValue(int id) 
{
	return move->getTriggerValue(id);
}

extern "C" __declspec(dllexport) void __stdcall setRumble(int id, int value) 
{
	move->setRumble(id, value);
}
