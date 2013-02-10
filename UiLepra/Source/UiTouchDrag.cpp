
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiTouchDrag.h"
#include "../Include/UiTouchstick.h"



namespace UiLepra
{
namespace Touch
{



Drag::Drag(int x, int y, bool pIsPress):
	mStart(x, y),
	mLast(x, y),
	mIsPress(pIsPress),
	mIsNew(true)
{
}

bool Drag::Update(const PixelCoord& pLast, const PixelCoord& pNew, bool pIsPress)
{
	mIsNew = false;

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
		i->Update(pPrevious, pLocation, pIsPressed);
	}
	mDragList.push_back(Drag(pLocation.x, pLocation.y, pIsPressed));
}

void DragManager::UpdateDragByMouse(const InputManager* pInputManager)
{
	PixelCoord lMouse;
	pInputManager->GetMousePosition(lMouse.x, lMouse.y);
	bool lIsPressed = pInputManager->GetMouse()->GetButton(0)->GetBooleanValue();
	UpdateDrag(mLastMouse, lMouse, lIsPressed);
	mLastMouse = lMouse;
}

void DragManager::UpdateMouseByDrag(InputManager* pInputManager)
{
	DragList::iterator i = mDragList.begin();
	for (; i != mDragList.end(); ++i)
	{
		if (i->mIsNew)
		{
			pInputManager->SetMousePosition(i->mLast.x, i->mLast.y);
			// We click-release to make it into an ordinary click-relese (button press).
			pInputManager->GetMouse()->GetButton(0)->SetValue(1);
			pInputManager->GetMouse()->GetButton(0)->SetValue(0);
		}
	}
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