
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <list>
#include "../../Lepra/Include/Graphics2D.h"
#include "UiLepra.h"



namespace Lepra
{
class Lock;
}


namespace UiLepra
{



class InputManager;



namespace Touch
{



using namespace Lepra;



enum DragFlags
{
	DRAGGING_STICK	= (1<<0),
	DRAGGING_UI	= (1<<1),
	DRAGGING_OTHER	= 0xFFFF,
	DRAG_USER	= (1<<16)
};

struct Drag
{
	Drag(int x, int y, bool isPress, int pButtonMask);
	void Update(const PixelCoord& pCoord, bool pIsPress, int pButtonMask);
	int GetDiamondDistanceTo(const PixelCoord& pCoord) const;

	PixelCoord mStart;
	PixelCoord mLast;
	bool mIsPress;
	bool mIsNew;
	int mButtonMask;
	uint32 mFlags;
	intptr_t mExtra;
};



class DragManager
{
public:
	typedef std::vector<Drag> DragList;

	DragManager();
	virtual ~DragManager();

	void SetMaxDragDistance(int pMaxDragDistance);
	void UpdateDrag(const PixelCoord& pPrevious, const PixelCoord& pLocation, bool pIsPressed, int pButtonMask);
	void UpdateDragByMouse(const InputManager* pInputManager);
	void UpdateMouseByDrag(InputManager* pInputManager);
	bool UpdateTouchsticks(InputManager* pInputManager) const;
	void SetDraggingUi(bool pIsUi);
	void DropReleasedDrags();
	void ClearDrags(InputManager* pInputManager);
	DragList GetDragList();

private:
	Lock* mLock;
	DragList mDragList;
	PixelCoord mLastMouse;
	bool mMouseLastPressed;
	int mMaxDragDiamondDistance;

	logclass();
};



}
}
