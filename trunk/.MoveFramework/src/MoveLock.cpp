#include "MoveLock.h"

namespace Move
{
	std::vector<CRITICAL_SECTION> MoveLock::crit;

	void MoveLock::init(int numMoves)
	{
		if (numMoves>crit.size())
		{
			int oldSize=crit.size();
			crit.resize(numMoves);
			for (int i=oldSize; i<numMoves; i++)
			{
				InitializeCriticalSection(&crit[i]);
			}
		}
		else if (numMoves<crit.size())
		{
			int oldSize=crit.size();
			for (int i=numMoves; i<oldSize; i++)
			{
				DeleteCriticalSection(&crit[i]);
			}
			crit.resize(numMoves);
		}
	}

	void MoveLock::deinit()
	{
		init(0);
	}
		
	MoveLock::MoveLock(int moveId)
	{
		this->moveId=moveId;
		released=false;
		EnterCriticalSection(&crit[moveId]);
	}

	void MoveLock::release()
	{
		if (!released)
			LeaveCriticalSection(&crit[moveId]);
		released=true;
	}

	MoveLock::~MoveLock()
	{
		if (!released)
			LeaveCriticalSection(&crit[moveId]);
	}
}