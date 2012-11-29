
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../Include/UiTouchDrag.h"
#include "../Include/UiTouchstick.h"



namespace UiLepra
{
namespace Touch
{



Drag::Drag(int x, int y):
	mStart(x, y),
	mLast(x, y),
	mIsPress(true)
{
}

bool Drag::Update(const PixelCoord& pLast, const PixelCoord& pNew, bool pIsPress)
{
	if (std::abs(mLast.x-pLast.x) < 44 && std::abs(mLast.y-pLast.y) < 44)
	{
		mLast = pNew;
		mIsPress = pIsPress;
		return true;
	}
	return false;
}



DragManager::DragManager()
{
}

DragManager::~DragManager()
{
}

void DragManager::UpdateDrag(const PixelCoord& pPrevious, const PixelCoord& pLocation, bool pIsPressed)
{
	DragList::iterator i = mDragList.begin();
	for (; i != mDragList.end(); ++i)
	{
		if (i->Update(pPrevious, pLocation, pIsPressed))
		{
			return;
		}
	}
	mDragList.push_back(Drag(pLocation.x, pLocation.y));
}

void DragManager::UpdateTouchsticks(InputManager* pInputManager) const
{
	DragList::const_iterator i = mDragList.begin();
	for (; i != mDragList.end(); ++i)
	{
		const Drag& lDrag = *i;
		TouchstickInputDevice* lTouchstick = TouchstickInputDevice::GetByCoordinate(pInputManager, lDrag.mStart);
		if (!lTouchstick)
		{
			lTouchstick = TouchstickInputDevice::GetByCoordinate(pInputManager, lDrag.mLast);
		}
		if (lTouchstick)
		{
			lTouchstick->SetTap(lDrag.mLast, lDrag.mIsPress);
		}
	}
}

void DragManager::DropReleasedDrags()
{
	DragList::iterator i = mDragList.begin();
	for (; i != mDragList.end();)
	{
		if (!i->mIsPress)
		{
			mDragList.erase(i++);
		}
		else
		{
			++i;
		}
	}
}



}
}
