
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
	DragList::iterator i = mDragList.begin();
	for (; i != mDragList.end(); ++i)
	{
		pInputManager->SetMousePosition(i->mLast.x, i->mLast.y);
		pInputManager->GetMouse()->GetButton(0)->SetValue(i->mIsPress);
	}
}

bool DragManager::UpdateTouchsticks(InputManager* pInputManager) const
{
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
	DragList::iterator i = mDragList.begin();
	for (; i != mDragList.end(); ++i)
	{
		i->mIsPress = pIsPress;
	}
}

void DragManager::DropReleasedDrags()
{
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

DragManager::DragList& DragManager::GetDragList()
{
	return mDragList;
}



LOG_CLASS_DEFINE(UI_INPUT, DragManager);



}
}
