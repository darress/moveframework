#pragma once

#include "MoveIncludes.h"

namespace Move
{
	struct MoveLock
	{
	private:
		static std::vector<CRITICAL_SECTION> crit;
		bool released;
		int moveId;

	public:
		static void init(int numMoves);
		static void deinit();
		
		MoveLock(int moveId);
		void release();
		~MoveLock();
	};
}