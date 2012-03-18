
#include "MovePrecompiled.h"
#include "IMoveManager.h"
#include "MoveFactory.h"
#include "MoveData.h"

Move::IMoveManager* move;
int numMoves;

typedef void (__stdcall *UPDATE_CALLBACK)(int id, float px, float py, float pz, float ow, float ox, float oy, float oz, int trigger);
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
		(*registered_callback)(
			moveId,
			data.position.x,data.position.y,data.position.z,
			data.orientation.w,data.orientation.x,data.orientation.y,data.orientation.z,
			data.trigger);
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