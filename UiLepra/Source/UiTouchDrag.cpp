
// Author: Jonas Byström
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
	mIsPress(pIsPress)
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
	if (mDragList.size() != 1)
	{
		return;
	}
	const Drag& lDrag = mDragList.front();
	pInputManager->SetMousePosition(lDrag.mLast.x, lDrag.mLast.y);
	if (lDrag.mIsPress)
	{
		pInputManager->GetMouse()->GetButton(0)->SetValue(1);
	}
	else
	{
		// If releasing, we click-release to make sure we don't miss anything.
		pInputManager->GetMouse()->GetButton(0)->SetValue(1);
		pInputManager->GetMouse()->GetButton(0)->SetValue(0);
	}
	//pInputManager->GetMouse()->GetAxis(0)->SetValue(lDrag.mLast.x);
	//pInputManager->GetMouse()->GetAxis(1)->SetValue(lDrag.mLast.y);
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
