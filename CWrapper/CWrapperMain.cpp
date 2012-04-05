
#include "IMoveManager.h"
#include "MoveFactory.h"
#include "MoveData.h"

Move::IMoveManager* move;
int numMoves;

typedef struct tagVec3
{ 
	float x;
	float y;
	float z;
} Vec3; 

typedef struct tagQuat
{ 
	float w;
	float x;
	float y;
	float z;
} Quat; 

typedef void (__stdcall *UPDATE_CALLBACK)(int id, Vec3 position, Quat orientation, int trigger);
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
		Vec3 pos;
		pos.x=data.position.x;
		pos.y=data.position.y;
		pos.z=data.position.z;

		Quat q;
		q.w=data.orientation.w;
		q.x=data.orientation.v.x;
		q.y=data.orientation.v.y;
		q.z=data.orientation.v.z;

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
	move->subsribe(observer);
}

extern "C" __declspec(dllexport) void __stdcall unsubscribeMove() 
{
	move->unsubsribe(observer);
}

extern "C" __declspec(dllexport) Quat __stdcall getOrientation(int id) 
{
	Move::MoveData data = move->getMove(id)->getMoveData();
	Move::Quat ori = data.orientation;
	Quat q;
	q.w=ori.w;
	q.x=ori.v.x;
	q.y=ori.v.y;
	q.z=ori.v.z;
	return q;
}

extern "C" __declspec(dllexport) Vec3 __stdcall getPosition(int id) 
{
	Move::MoveData data = move->getMove(id)->getMoveData();
	Move::Vec3 pos = data.position;
	Vec3 v;
	v.x=pos.x;
	v.y=pos.y;
	v.z=pos.z;
	return v;
}

extern "C" __declspec(dllexport) bool __stdcall getButtonState(int id, int keyId) 
{
	Move::MoveData data = move->getMove(id)->getMoveData();
	return data.isButtonPressed((Move::MoveButton)keyId);
}

extern "C" __declspec(dllexport) int __stdcall getTriggerValue(int id) 
{
	Move::MoveData data = move->getMove(id)->getMoveData();
	return data.trigger;
}

extern "C" __declspec(dllexport) void __stdcall setRumble(int id, int value) 
{
	move->getMove(id)->setRumble(value);
}
