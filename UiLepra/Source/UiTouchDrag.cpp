
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



Drag::Drag(int x, int y, bool pIsPress, int pButtonMask):
	mStart(x, y),
	mLast(x, y),
	mIsPress(pIsPress),
	mIsNew(true),
	mButtonMask(pButtonMask),
	mFlags(0)
{
}

void Drag::Update(const PixelCoord& pCoord, bool pIsPress, int pButtonMask)
{
	mIsNew = false;
	mLast = pCoord;
	mIsPress = pIsPress;
	mButtonMask = pButtonMask;
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

void DragManager::UpdateDrag(const PixelCoord& pPrevious, const PixelCoord& pLocation, bool pIsPressed, int pButtonMask)
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
		lBestDrag->Update(pLocation, pIsPressed, pButtonMask);
		return;
	}
	mDragList.push_back(Drag(pLocation.x, pLocation.y, pIsPressed, pButtonMask));
}

void DragManager::UpdateDragByMouse(const InputManager* pInputManager)
{
	ScopeLock lLock(mLock);
	PixelCoord lMouse;
	pInputManager->GetMousePosition(lMouse.x, lMouse.y);
	int lButtonMask = 0;
	const int c = pInputManager->GetMouse()->GetNumDigitalElements();
	for (int x = 0; x < c; ++x)
	{
		lButtonMask |= ((pInputManager->GetMouse()->GetButton(x)->GetBooleanValue()? 1:0) << x);
	}
	bool lIsPressed = !!lButtonMask;
	if (lIsPressed || mMouseLastPressed)
	{
		UpdateDrag(mLastMouse, lMouse, lIsPressed, lButtonMask);
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
		int lButtonMask = i->mButtonMask;
		const int c = pInputManager->GetMouse()->GetNumDigitalElements();
		for (int x = 0; x < c; ++x)
		{
			pInputManager->GetMouse()->GetButton(x)->SetValue((lButtonMask&(1<<x))? 1 : 0);
		}
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
		if (pIsPress)
		{
			i->mButtonMask |= 1;
		}
		else
		{
			i->mButtonMask = 0;
		}
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
		const int c = pInputManager->GetMouse()->GetNumDigitalElements();
		for (int x = 0; x < c; ++x)
		{
			pInputManager->GetMouse()->GetButton(x)->SetValue(0);
		}
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
