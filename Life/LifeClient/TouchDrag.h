
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Lepra/Include/HiResTimer.h"
#include <list>
#include "../Life.h"



namespace Life
{
namespace Touch
{



struct Drag
{
	int mStartX;
	int mStartY;
	int mLastX;
	int mLastY;
	int mMovedDistance;
	bool mIsPress;
	int mTag;
	HiResTimer mTimer;

	inline Drag(int x, int y):
		mStartX(x),
		mStartY(y),
		mLastX(x),
		mLastY(y),
		mMovedDistance(0),
		mIsPress(true),
		mTag(0),
		mTimer(false)
	{
	}

	inline bool Update(int pLastX, int pLastY, int pNewX, int pNewY)
	{
		if (std::abs(mLastX-pLastX) < 44 && std::abs(mLastY-pLastY) < 44)
		{
			mMovedDistance += std::abs(mLastX-pNewX);
			mMovedDistance += std::abs(mLastY-pNewY);
			mLastX = pNewX;
			mLastY = pNewY;
			return true;
		}
		return false;
	}
};

typedef std::list<Drag> DragList;

DragList gDragList;



}
}
