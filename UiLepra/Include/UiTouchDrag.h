
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <list>
#include "../../Lepra/Include/Graphics2D.h"
#include "UiLepra.h"



namespace UiLepra
{



class InputDevice;
class InputManager;



namespace Touch
{



using namespace Lepra;



struct Drag
{
	Drag(int x, int y);
	bool Update(const PixelCoord& pLast, const PixelCoord& pNew, bool pIsPress);

	PixelCoord mStart;
	PixelCoord mLast;
	bool mIsPress;

};



class DragManager
{
	DragManager();
	virtual ~DragManager();

	void UpdateDrag(const PixelCoord& pPrevious, const PixelCoord& pLocation, bool pIsPressed);
	void UpdateDragByMouse(InputDevice* pMouse) const;
	void UpdateTouchsticks(InputManager* pInputManager) const;
	void DropReleasedDrags();

private:
	typedef std::list<Drag> DragList;
	DragList mDragList;
};



}
}
