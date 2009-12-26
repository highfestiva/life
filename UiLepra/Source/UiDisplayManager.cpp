
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiDisplayManager.h"
#include <assert.h>



namespace UiLepra
{



DisplayManager::DisplayManager()
{
	AddResizeObserver(this);
}

DisplayManager::~DisplayManager()
{
	RemoveResizeObserver(this);
	assert(mResizeObserverSet.empty());
}



void DisplayManager::AddResizeObserver(DisplayResizeObserver* pResizeObserver)
{
	mResizeObserverSet.insert(pResizeObserver);
}

void DisplayManager::RemoveResizeObserver(DisplayResizeObserver* pResizeObserver)
{
	mResizeObserverSet.erase(pResizeObserver);
}

void DisplayManager::DispatchResize(int pWidth, int pHeight)
{
	ResizeObserverSet::iterator x = mResizeObserverSet.begin();
	for (; x != mResizeObserverSet.end(); ++x)
	{
		(*x)->OnResize(pWidth, pHeight);
	}
}

void DisplayManager::DispatchMinimize()
{
	ResizeObserverSet::iterator x = mResizeObserverSet.begin();
	for (; x != mResizeObserverSet.end(); ++x)
	{
		(*x)->OnMinimize();
	}
}

void DisplayManager::DispatchMaximize(int pWidth, int pHeight)
{
	ResizeObserverSet::iterator x = mResizeObserverSet.begin();
	for (; x != mResizeObserverSet.end(); ++x)
	{
		(*x)->OnMaximize(pWidth, pHeight);
	}
}



void DisplayManager::GetScreenCanvas(Canvas& pCanvas)
{
	pCanvas.Reset(GetWidth(), GetHeight(), Canvas::IntToBitDepth(GetBitDepth()));
	pCanvas.SetPitch(GetPitch());
}



LOG_CLASS_DEFINE(UI_GFX, DisplayManager);



}
