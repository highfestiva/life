
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
	bool Update(const PixelCoord& pLast, const PixelCoord& pNew, bool pIsPress);

	PixelCoord mStart;
	PixelCoord mLast;
	bool mIsPress;
	bool mIsNew;
};



class DragManager
{
public:
	typedef std::list<Drag> DragList;

	DragManager();
	virtual ~DragManager();

	void UpdateDrag(const PixelCoord& pPrevious, const PixelCoord& pLocation, bool pIsPressed);
	void UpdateDragByMouse(const InputManager* pInputManager);
	void UpdateMouseByDrag(InputManager* pInputManager);
	void UpdateTouchsticks(InputManager* pInputManager) const;
	void DropReleasedDrags();
	const DragList& GetDragList() const;

private:
	DragList mDragList;
	PixelCoord mLastMouse;
	bool mLastPressed;
};



}
}
