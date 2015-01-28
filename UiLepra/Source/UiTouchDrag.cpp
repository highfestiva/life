
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/UiTouchDrag.h"
#include "../../Lepra/Include/Thread.h"
#include "../Include/UiTouchstick.h"



namespace UiLepra
{
namespace Touch
{



Drag::Drag(int x, int y, bool pIsPress):
	mStart(x, y),
	mLast(x, y),
	mIsPress(pIsPress),
	mIsNew(true),
	mFlags(0)
{
}

void Drag::Update(const PixelCoord& pCoord, bool pIsPress)
{
	mIsNew = false;
	mLast = pCoord;
	mIsPress = pIsPress;
}

int Drag::GetDiamondDistanceTo(const PixelCoord& pCoord) const
{
	return std::abs(mLast.x-pCoord.x) + std::abs(mLast.y-pCoord.y);
}



DragManager::DragManager():
	mLock(new Lock),
	mMouseLastPressed(false),
	mMaxDragDiamondDistance(106)
{
}

DragManager::~DragManager()
{
}

void DragManager::SetMaxDragDistance(int pMaxDragDistance)
{
	mMaxDragDiamondDistance = (int)(pMaxDragDistance*1.2f);
}

void DragManager::UpdateDrag(const PixelCoord& pPrevious, const PixelCoord& pLocation, bool pIsPressed)
{
	ScopeLock lLock(mLock);
	int lClosestDiamondDistance = 1000000;
	DragList::iterator i = mDragList.begin();
	DragList::iterator lBestDrag = i;
	for (; i != mDragList.end(); ++i)
	{
		int d = i->GetDiamondDistanceTo(pPrevious);
		if (d < lClosestDiamondDistance)
		{
			lClosestDiamondDistance = d;
			lBestDrag = i;
		}
	}
	if (lBestDrag != mDragList.end() && (!pIsPressed || lClosestDiamondDistance <= mMaxDragDiamondDistance))	// If releasing we must do this.
	{
		lBestDrag->Update(pLocation, pIsPressed);
		return;
	}
	mDragList.push_back(Drag(pLocation.x, pLocation.y, pIsPressed));
}

void DragManager::UpdateDragByMouse(const InputManager* pInputManager)
{
	ScopeLock lLock(mLock);
	PixelCoord lMouse;
	pInputManager->GetMousePosition(lMouse.x, lMouse.y);
	bool lIsPressed = pInputManager->GetMouse()->GetButton(0)->GetBooleanValue();
	if (lIsPressed || mMouseLastPressed)
	{
		UpdateDrag(mLastMouse, lMouse, lIsPressed);
	}
	mLastMouse = lMouse;
	mMouseLastPressed = lIsPressed;
}

void DragManager::UpdateMouseByDrag(InputManager* pInputManager)
{
	ScopeLock lLock(mLock);
	DragList::iterator i = mDragList.begin();
	for (; i != mDragList.end(); ++i)
	{
		pInputManager->SetMousePosition(i->mLast.x, i->mLast.y);
		pInputManager->GetMouse()->GetButton(0)->SetValue(i->mIsPress? 1 : 0);
	}
}

bool DragManager::UpdateTouchsticks(InputManager* pInputManager) const
{
	ScopeLock lLock(mLock);
	bool lDidUseStick = false;
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
			lDidUseStick = true;
		}
	}
	return lDidUseStick;
}

void DragManager::SetDragsPress(bool pIsPress)
{
	ScopeLock lLock(mLock);
	DragList::iterator i = mDragList.begin();
	for (; i != mDragList.end(); ++i)
	{
		i->mIsPress = pIsPress;
	}
}

void DragManager::DropReleasedDrags()
{
	ScopeLock lLock(mLock);
	DragList::iterator i = mDragList.begin();
	for (; i != mDragList.end();)
	{
		if (!i->mIsPress)
		{
			i = mDragList.erase(i);
		}
		else
		{
			++i;
		}
	}
}

void DragManager::ClearDrags(InputManager* pInputManager)
{
	ScopeLock lLock(mLock);
	mDragList.clear();
	if (pInputManager->GetMouse())
	{
		pInputManager->GetMouse()->GetButton(0)->SetValue(0);
	}
}

DragManager::DragList DragManager::GetDragList()
{
	ScopeLock lLock(mLock);
	return mDragList;
}



loginstance(UI_INPUT, DragManager);



}
}
