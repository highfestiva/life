
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <list>
#include "../../Lepra/Include/Graphics2D.h"
#include "UiLepra.h"



namespace UiLepra
{



class InputManager;



namespace Touch
{



using namespace Lepra;



struct Drag
{
	Drag(int x, int y, bool isPress);
	void Update(const PixelCoord& pCoord, bool pIsPress);
	int GetDiamondDistanceTo(const PixelCoord& pCoord) const;

	PixelCoord mStart;
	PixelCoord mLast;
	bool mIsPress;
	bool mIsNew;
	uint32 mFlags;
};



class DragManager
{
public:
	typedef std::vector<Drag> DragList;

	DragManager();
	virtual ~DragManager();

	void SetMaxDragDistance(int pMaxDragDistance);
	void UpdateDrag(const PixelCoord& pPrevious, const PixelCoord& pLocation, bool pIsPressed);
	void UpdateDragByMouse(const InputManager* pInputManager);
	void UpdateMouseByDrag(InputManager* pInputManager);
	bool UpdateTouchsticks(InputManager* pInputManager) const;
	void SetDragsPress(bool pIsPress);
	void DropReleasedDrags();
	DragList& GetDragList();

private:
	DragList mDragList;
	PixelCoord mLastMouse;
	bool mMouseLastPressed;
	int mMaxDragDiamondDistance;

	logclass();
};



}
}
